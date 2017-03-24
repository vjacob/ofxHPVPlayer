/**********************************************************
* Holo_ToolSet
* http://github.com/HasseltVR/Holo_ToolSet
* http://www.uhasselt.be/edm
*
* Distributed under LGPL v2.1 Licence
* http ://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
**********************************************************/
#pragma once

#include <string>
#include <map>
#include <stdint.h>

#include "Log.h"
#include "HPVManager.h"
#include "ofMain.h"

#define BACK 0
#define FRONT 1

namespace HPV {

    struct HPVRenderData;
    
    typedef std::function<void(HPVRenderData * const)> HPVRenderFunc;

    /*
    * HPVRenderType enum specifies a render backend for the engine
    */
    enum class HPVRendererType : std::uint8_t
    {
        RENDERER_NONE,
        RENDERER_OPENGLCORE,
    };
    
    enum class HPVRenderState : std::uint8_t
    {
        STATE_BUFFER = 0,
        STATE_STREAM,
        STATE_BLIT,
        NUM_RENDER_STATES = 3
    };

    /*
    * OpenGLTexData: the necessary data to use OpenGL resources
    */
    struct OpenGLTexData
    {
        /* OpenGL texture handle */
        GLuint tex = 0;

        /* OpenGL Pixel Buffer Object handles (double-buffered) */
        GLuint pboIds[2] = { 0 };

        /* The gl pixel format for this file */
        GLenum gl_format;

        /* The current fill index (in case of using PBO) */
        uint8_t tex_fill_index = 0;
    };

    /*
    * OpenGLTexData: the necessary data to use OpenGL resources
    */
    struct HPVRenderStats
    {
        uint64_t before_upload = 0;
        uint64_t after_upload = 0;
    };

    /*
    *   HPVRenderData: allocted for each player, containing pointers 
    *   to all GPU resources
    */
    struct HPVRenderData
    {
        /* Pointer to the player */
        HPVPlayerRef player;

        /* OpenGL */
        OpenGLTexData opengl;
        
        /* DirectX */

        /* Abstract Render state (buffer, stream, blit) */
        HPVRenderState render_state;
        
        /* Abstract Render function */
        HPVRenderFunc render_func;
         
        /* Abstract Render Statistics */
        HPVRenderStats stats;
        
        bool gpu_resources_need_init;
        bool needs_buffer;
        uint32_t cpu_framenum;
        uint32_t gpu_framenum;

        HPVRenderData()
        {
            gpu_resources_need_init = true;
            cpu_framenum = 0;
            gpu_framenum = 0;
        }
    };
    

    /*
    *   HPVRenderBridge: providing a bridge between the HPV Player CPU backend 
    *   and GPU pixels sink
    */
    class HPVRenderBridge
    {
    public:
        HPVRenderBridge();
        ~HPVRenderBridge();

        void load();
        void unload();

        int initPlayer(uint8_t node_id);
        void setRenderer(HPVRendererType renderer);
        HPVRendererType getRenderer();

        int createGPUResources(uint8_t node_id);
        int deleteGPUResources();
        int nodeHasResources(uint8_t node_id);
        intptr_t getTexturePtr(uint8_t node_id);
        
        GLenum getGLInternalFormat(uint8_t node_id);

        void updateTextures();
        uint32_t getCPUFrameForNode(uint8_t);
        uint32_t getGPUFrameForNode(uint8_t);

        bool s3tc_supported;
        bool pbo_supported;
        
        void buffer_func(HPVRenderData * const);
        void stream_func(HPVRenderData * const);
        void blit_func(HPVRenderData * const);
        
        HPVRenderState getRenderState(uint8_t node_idx);
        void setRenderState(uint8_t node_idx, HPVRenderState state);
        
        bool needsBuffering(uint8_t node_idx);
                
    private:
        HPVRendererType m_renderer;
        HPVRenderFunc m_render_func;
        HPVRenderFunc m_render_funcs[(uint8_t)HPVRenderState::NUM_RENDER_STATES];
        std::map<uint8_t, HPVRenderData> m_render_data;

        bool b_needs_buffer;
    };
        
    /*
     * HPVRenderer singleton instance
     */
    HPVRenderBridge * RendererSingleton();
} /* End HPV namespace */

/* The HPV Renderer Singleton */
extern HPV::HPVRenderBridge m_HPVRenderer;