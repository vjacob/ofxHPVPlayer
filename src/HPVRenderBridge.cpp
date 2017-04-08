#include "HPVRenderBridge.h"

#ifndef __forceinline
#define __forceinline __attribute__((always_inline))
#endif

namespace HPV {

    /* The HPV Renderer Singleton */
    HPVRenderBridge m_HPVRenderer;

    static void ReportGLError()
    {
        GLenum err = GL_NO_ERROR;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            if (err == GL_INVALID_ENUM)
            {
                HPV_ERROR("GL_INVALID_ENUM");
            }
            else if (err == GL_INVALID_VALUE)
            {
                HPV_ERROR("GL_INVALID_VALUE");
            }
            else if (err == GL_INVALID_OPERATION)
            {
                HPV_ERROR("GL_INVALID_OPERATION");
            }
            else
            {
                HPV_ERROR("Other GL Error");
            }
        }
    }

    HPVRenderBridge::HPVRenderBridge() : m_renderer(HPVRendererType::RENDERER_NONE)
    {
        s3tc_supported = false;
        pbo_supported = false;
    }

    HPVRenderBridge::~HPVRenderBridge()
    {
        HPV_VERBOSE("~RenderBridge");
    }

    void HPVRenderBridge::setRenderer(HPVRendererType renderer)
    {
        m_renderer = renderer;
    }

    void HPVRenderBridge::load()
    {
        setRenderer(HPVRendererType::RENDERER_OPENGLCORE);
        
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        HPV_VERBOSE("Using GLEW %s with OpenGL %s", glewGetString(GLEW_VERSION), glGetString(GL_VERSION));

        if (major != 4 || (major == 4 && minor < 1) )
        {
            throw std::runtime_error("OpenGL version too low, the HPV eco-system needs at least an OpenGL 4.1 context");
        }
        
        /* We need at least an OpenGL 4.1 context, these extensions should be supported */
        s3tc_supported = true;
        pbo_supported = true;
        
        using namespace std::placeholders;
        m_render_funcs[(uint8_t)HPV::HPVRenderState::STATE_BUFFER] = std::bind(&HPV::HPVRenderBridge::buffer_func, this, _1);
        m_render_funcs[(uint8_t)HPV::HPVRenderState::STATE_STREAM] = std::bind(&HPV::HPVRenderBridge::stream_func, this, _1);
        m_render_funcs[(uint8_t)HPV::HPVRenderState::STATE_BLIT]   = std::bind(&HPV::HPVRenderBridge::blit_func,   this, _1);
    }
    
    void HPVRenderBridge::unload()
    {
        setRenderer(HPVRendererType::RENDERER_NONE);
    }

    int HPVRenderBridge::initPlayer(uint8_t node_id)
    {
        m_render_data.insert(std::pair<uint8_t, HPVRenderData>(node_id, HPVRenderData()));
        return HPV_RET_ERROR_NONE;
    }

    int HPVRenderBridge::createGPUResources(uint8_t node_id)
    {
        if (m_render_data.find(node_id) == m_render_data.end())
        {
            HPV_ERROR("Can't create resources, player %d was not allocated!", node_id);
            return HPV_RET_ERROR;
        }

        HPVRenderData& data = m_render_data.at(node_id);
        data.player = ManagerSingleton()->getPlayer(node_id);
        data.stats.after_upload = 0;
        data.stats.before_upload = 0;
        data.gpu_resources_need_init = true;

        if (HPVRendererType::RENDERER_OPENGLCORE == m_renderer)
        {
            HPVCompressionType ct = data.player->getCompressionType();
            
            switch (ct) {
                case HPVCompressionType::HPV_TYPE_DXT1_NO_ALPHA:
                    data.opengl.gl_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                    break;
                
                case HPVCompressionType::HPV_TYPE_DXT5_ALPHA:
                    data.opengl.gl_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                    break;
                
                case HPVCompressionType::HPV_TYPE_SCALED_DXT5_CoCg_Y:
                    data.opengl.gl_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                    break;
                    
                default:
                    HPV_ERROR("HPV::Unrecognised compression type!");
                    break;
            }

            glGenTextures(1, &data.opengl.tex);
            
            glBindTexture(GL_TEXTURE_2D, data.opengl.tex);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // allocate texture storage for this texture
            glTexStorage2D(GL_TEXTURE_2D, 1, data.opengl.gl_format, data.player->getWidth(), data.player->getHeight());

            if (pbo_supported)
            {
                glGenBuffers(2, data.opengl.pboIds);
                
                data.needs_buffer = true;
            }
            
            glBindTexture(GL_TEXTURE_2D, 0);

            data.gpu_resources_need_init = false;
            
            this->setRenderState(node_id, HPVRenderState::STATE_BLIT);

            ReportGLError();

            return HPV_RET_ERROR_NONE;
        }

        return HPV_RET_ERROR_NONE;
    }

    int HPVRenderBridge::nodeHasResources(uint8_t node_id)
    {
        return static_cast<int>(!m_render_data[node_id].gpu_resources_need_init);
    }

    int HPVRenderBridge::deleteGPUResources()
    {
        for (auto& data : m_render_data)
        {
            if (data.second.gpu_resources_need_init)
            {
                continue;
            }

            if (HPVRendererType::RENDERER_OPENGLCORE == m_renderer)
            {
                glDeleteTextures(1, &data.second.opengl.tex);
                glDeleteBuffers(2, &data.second.opengl.pboIds[0]);
            }
        }

        m_render_data.clear();

        HPV_VERBOSE("Deleted GPU resources");

        return HPV_RET_ERROR_NONE;
    }

    intptr_t HPVRenderBridge::getTexturePtr(uint8_t node_id)
    {
        if (HPVRendererType::RENDERER_OPENGLCORE == m_renderer)
        {
            return m_render_data[node_id].opengl.tex;
        }
        else return 0;
    }
    
    GLenum HPVRenderBridge::getGLInternalFormat(uint8_t node_id)
    {
        return (m_render_data[node_id].opengl.gl_format);
    }

    HPVRendererType HPVRenderBridge::getRenderer()
    {
        return m_renderer;
    }
    
    void HPVRenderBridge::buffer_func(HPVRenderData * const data)
    {
        if (!data->player || !data->player->isLoaded())
        {
            return;
        }
        
        HPV_VERBOSE("HPV::Buffering...");
        
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data->opengl.pboIds[BACK]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, data->player->getBytesPerFrame(), data->player->getBufferPtr(), GL_STREAM_DRAW);
        
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data->opengl.pboIds[FRONT]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, data->player->getBytesPerFrame(), 0, GL_STREAM_DRAW);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        
        data->cpu_framenum = data->player->getCurrentFrameNumber();
        data->opengl.tex_fill_index = FRONT;
        data->render_state = HPVRenderState::STATE_STREAM;
        data->render_func = m_render_funcs[(uint8_t)data->render_state];
        data->needs_buffer = false;
    }
    
    void HPVRenderBridge::stream_func(HPV::HPVRenderData *const data)
    {
        if (!data->player || !data->player->isLoaded())
        {
            return;
        }
        
        int pbo_fill_index = 0;
        
        // tex_fill_index is used to copy pixels from a PBO to a texture object
        // pbo_fill_index is used to update pixels in a PBO
        data->opengl.tex_fill_index = (data->opengl.tex_fill_index + 1) % 2;
        pbo_fill_index = (data->opengl.tex_fill_index + 1) % 2;
        
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data->opengl.pboIds[data->opengl.tex_fill_index]);
        
        // don't use pointer for uploading data (last parameter = 0), data will come from bound PBO
        glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data->player->getWidth(), data->player->getHeight(), data->opengl.gl_format, static_cast<GLsizei>(data->player->getBytesPerFrame()), 0);
        
        data->gpu_framenum = data->cpu_framenum;
        
        // bind PBO to update pixel values
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data->opengl.pboIds[pbo_fill_index]);
        
        // map pointer for memcpy from our frame buffer, invalidate and 'orphan' this buffer. The OpenGL implementation will clean the buffer
        // when it has done reading its values, but in the meantime we've allocated new storage for writing new values, resulting in a
        // un-synchronized workflow, which is exactly what we want
        // https://www.khronos.org/opengl/wiki/Buffer_Object#Invalidation
        GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, data->player->getBytesPerFrame(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        
        if (ptr)
        {
            data->cpu_framenum = data->player->getCurrentFrameNumber();
            memcpy(ptr, data->player->getBufferPtr(), data->player->getBytesPerFrame());
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        }
        
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
    
    void HPVRenderBridge::blit_func(HPV::HPVRenderData *const data)
    {
        if (!data->player || !data->player->isLoaded())
        {
            return;
        }
        
        glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data->player->getWidth(), data->player->getHeight(), data->opengl.gl_format, static_cast<GLsizei>(data->player->getBytesPerFrame()), data->player->getBufferPtr());
        
        data->cpu_framenum = data->player->getCurrentFrameNumber();
        data->gpu_framenum = data->cpu_framenum;
    }
    
    void HPVRenderBridge::setRenderState(uint8_t node_idx, HPVRenderState state)
    {
        if (!m_render_data.count(node_idx))
        {
            return;
        }
        else if (state >= HPVRenderState::NUM_RENDER_STATES)
        {
            return;
        }
        
        HPVRenderData& render_data = m_render_data[node_idx];
        
        switch (state) {
            case HPVRenderState::STATE_BUFFER:
            case HPVRenderState::STATE_STREAM:
            {
                render_data.render_state = HPVRenderState::STATE_BUFFER;
                render_data.render_func = m_render_funcs[(uint8_t)render_data.render_state];
                render_data.needs_buffer = true;
            }
            break;

            case HPVRenderState::STATE_BLIT:
            default:
            {
                render_data.render_state = HPVRenderState::STATE_BLIT;
                render_data.render_func = m_render_funcs[(uint8_t)render_data.render_state];
                render_data.needs_buffer = false;
            }
            break;
        }
        
        if (render_data.render_state == HPVRenderState::STATE_BUFFER)
        {
            render_data.render_func(&render_data);
        }
    }

    void HPVRenderBridge::updateTextures()
    {
        std::vector<bool> update_flags = ManagerSingleton()->update();

        for (uint8_t player_idx = 0; player_idx < update_flags.size(); ++player_idx)
        {
            if (update_flags[player_idx])
            {
                /* Get specifics for this player */
                HPVRenderData& render_data = m_render_data[player_idx];

                if (render_data.gpu_resources_need_init)
                    return;

                if (HPVRendererType::RENDERER_OPENGLCORE == m_renderer)
                {
                    // be sure to unbind any unpack buffer before start
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                    glBindTexture(GL_TEXTURE_2D, render_data.opengl.tex);

                    if (render_data.player->_gather_stats) render_data.stats.before_upload = ns();
                    
                    /* Main pixel upload func */
                    render_data.render_func(&render_data);
                                        
                    glBindTexture(GL_TEXTURE_2D, 0);

                    if (render_data.player->_gather_stats)
                    {
                        render_data.stats.after_upload = ns();
                        render_data.player->_decode_stats.gpu_upload_time = render_data.stats.after_upload - render_data.stats.before_upload;
                    }
                }
                
                if (render_data.player->isStopped() && render_data.gpu_framenum != render_data.player->getCurrentFrameNumber())
                {
                    update_flags[player_idx] = true;
                }
            }
        }
    }
    
    uint32_t HPVRenderBridge::getCPUFrameForNode(uint8_t node_idx)
    {
        if (m_render_data.count(node_idx))
        {
            return m_render_data[node_idx].cpu_framenum;
        }
        else
        {
            return 0;
        }
    }
    
    uint32_t HPVRenderBridge::getGPUFrameForNode(uint8_t node_idx)
    {
        if (m_render_data.count(node_idx))
        {
            return m_render_data[node_idx].gpu_framenum;
        }
        else
        {
            return 0;
        }
    }
    
    HPVRenderState HPVRenderBridge::getRenderState(uint8_t node_idx)
    {
        if (m_render_data.count(node_idx))
        {
            return m_render_data[node_idx].render_state;
        }
        
        return HPVRenderState::NUM_RENDER_STATES;
    }
    
    bool HPVRenderBridge::needsBuffering(uint8_t node_idx)
    {
        if (m_render_data.count(node_idx))
        {
            return m_render_data[node_idx].needs_buffer;
        }
        else
        {
            return false;
        }
    }
    
    /*******************************************************************************
     * GLOBAL Renderer functions
     *******************************************************************************/
    HPVRenderBridge * RendererSingleton()
    {
        return &m_HPVRenderer;
    }
} /* End HPV namespace */
