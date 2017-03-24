#include "ofxHPVPlayer.h"

static const GLchar* vert_CT_CoCg_Y = R"(
    #version 410

    // addressed by OF
    uniform mat4 modelViewMatrix;
    uniform mat4 projectionMatrix;
    uniform mat4 textureMatrix;
    uniform mat4 modelViewProjectionMatrix;

    layout (location = 0) in vec4 position;
    layout (location = 1) in vec4 color;
    layout (location = 2) in vec4 normal;
    layout (location = 3) in vec2 texcoord;
    // end addressed by OF

    out vec2 tc;

    void main()
    {
        gl_Position = modelViewProjectionMatrix * position;
        tc = texcoord;
    }
)";

static const GLchar* frag_CT_CoCg_Y = R"(
    #version 410

    const vec4 offsets = vec4(0.50196078431373, 0.50196078431373, 0.0, 0.0);
    const float scale_factor = 255.0 / 8.0;

    uniform sampler2D hpv_tex;
    in vec2 tc;

    out vec4 outputColor;

    void main()
    {
        vec4 rgba = texture(hpv_tex, tc);
        
        rgba -= offsets;
        
        float Y = rgba.a;
        float scale = rgba.b * scale_factor + 1;
        float Co = rgba.r / scale;
        float Cg = rgba.g / scale;
        
        outputColor = vec4(Y + Co - Cg, Y + Cg, Y - Co - Cg, 1);
    }
)";

ofxHPVPlayer::ofxHPVPlayer()
{

}

ofxHPVPlayer::~ofxHPVPlayer()
{
}

void ofxHPVPlayer::init(HPVPlayerRef internal_hpv_player)
{
    m_hpv_player = internal_hpv_player;
    RendererSingleton()->initPlayer(m_hpv_player->getID());
}

////////////////////////////////////////////////////////////////////////
// HPV specific functions
////////////////////////////////////////////////////////////////////////
// Opens the video file
bool ofxHPVPlayer::load(string name)
{
    int ret = m_hpv_player->open(ofToDataPath(name, true).c_str());

    if (ret == HPV_RET_ERROR_NONE)
    {
        ret = RendererSingleton()->createGPUResources(m_hpv_player->getID());
        
        if (ret == HPV_RET_ERROR_NONE)
        {
            GLuint tex_id = RendererSingleton()->getTexturePtr(m_hpv_player->getID());
            m_texture.clear();
            m_texture.setUseExternalTextureID(tex_id);
            m_texture.texData.width = m_hpv_player->getWidth();
            m_texture.texData.height = m_hpv_player->getHeight();
            m_texture.texData.tex_w = m_hpv_player->getWidth();
            m_texture.texData.tex_h = m_hpv_player->getHeight();
            m_texture.texData.tex_u = 1;
            m_texture.texData.tex_t = 1;
            m_texture.texData.bFlipTexture = false;
            m_texture.texData.glInternalFormat = RendererSingleton()->getGLInternalFormat(m_hpv_player->getID());
            m_texture.texData.textureTarget = GL_TEXTURE_2D;
            
            if (m_hpv_player->getCompressionType() == HPVCompressionType::HPV_TYPE_SCALED_DXT5_CoCg_Y)
            {
                m_shader.setupShaderFromSource(GL_VERTEX_SHADER, vert_CT_CoCg_Y);
                m_shader.setupShaderFromSource(GL_FRAGMENT_SHADER, frag_CT_CoCg_Y);
                m_shader.linkProgram();
            }
        }
        else
        {
            HPV_ERROR("Couldn't create OpenGL resources for '%s'", name.c_str());
        }
    }

    return ret;
}

bool ofxHPVPlayer::loadAsync(string name)
{
    return this->load(name);
}

void ofxHPVPlayer::play()
{
    if (needsDoubleBuffering())
    {
        RendererSingleton()->setRenderState(m_hpv_player->getID(), HPVRenderState::STATE_BUFFER);
    }

    m_hpv_player->play();
}

void ofxHPVPlayer::stop()
{
    if (needsDoubleBuffering())
    {
        RendererSingleton()->setRenderState(m_hpv_player->getID(), HPVRenderState::STATE_BLIT);
    }
    
    m_hpv_player->stop();
}

ofTexture * ofxHPVPlayer::getTexturePtr()
{
    return &m_texture;
}

float ofxHPVPlayer::getWidth() const
{
    return m_hpv_player->getWidth();
}

float ofxHPVPlayer::getHeight() const
{
    return m_hpv_player->getHeight();
}

bool ofxHPVPlayer::isPaused() const
{
    return static_cast<bool>(m_hpv_player->isPaused());
}

bool ofxHPVPlayer::isLoaded() const
{
    return static_cast<bool>(m_hpv_player->isLoaded());
}

bool ofxHPVPlayer::isPlaying() const
{
    return static_cast<bool>(m_hpv_player->isPlaying());
}

float ofxHPVPlayer::getPosition() const
{
    return m_hpv_player->getPosition();
}

float ofxHPVPlayer::getSpeed() const
{
    return m_hpv_player->getSpeed();
}

float ofxHPVPlayer::getDuration() const
{
    return (m_hpv_player->getNumberOfFrames()-1) / static_cast<float>(m_hpv_player->getFrameRate());
}

bool ofxHPVPlayer::getIsMovieDone() const
{
    return m_hpv_player->isStopped();
}

void ofxHPVPlayer::setPaused(bool bPause)
{
    if (bPause)
    {
        m_hpv_player->pause();
        
        if (needsDoubleBuffering())
        {
            RendererSingleton()->setRenderState(m_hpv_player->getID(), HPVRenderState::STATE_BLIT);
        }
    }
    else
    {
        m_hpv_player->seek(static_cast<int64_t>(RendererSingleton()->getCPUFrameForNode(m_hpv_player->_id)));
        
        if (needsDoubleBuffering())
        {
            RendererSingleton()->setRenderState(m_hpv_player->getID(), HPVRenderState::STATE_BUFFER);
        }
        
        m_hpv_player->resume();
    }
}

void ofxHPVPlayer::setPosition(float pct)
{
    m_hpv_player->seek(static_cast<double>(pct));
}

void ofxHPVPlayer::setVolume(float volume)
{
    HPV_ERROR("Setting volume is not supported yet");
}

void ofxHPVPlayer::setLoopState(ofLoopType state)
{
    switch (state)
    {
        case OF_LOOP_NONE:
            m_hpv_player->setLoopMode(HPV_LOOPMODE_NONE);
            break;
        case OF_LOOP_NORMAL:
            m_hpv_player->setLoopMode(HPV_LOOPMODE_LOOP);
            break;
        case OF_LOOP_PALINDROME:
            m_hpv_player->setLoopMode(HPV_LOOPMODE_PALINDROME);
            break;
            
        default:
            break;
    }
}

void ofxHPVPlayer::setSpeed(float speed)
{
    m_hpv_player->setSpeed(static_cast<double>(speed));
}

void ofxHPVPlayer::setFrame(int frame)
{
    m_hpv_player->seek(static_cast<int64_t>(frame));
}

int ofxHPVPlayer::getCurrentFrame() const
{
    return static_cast<int>(RendererSingleton()->getGPUFrameForNode(m_hpv_player->_id));
}

int ofxHPVPlayer::getTotalNumFrames() const
{
    return static_cast<int>(m_hpv_player->getNumberOfFrames());
}

ofLoopType ofxHPVPlayer::getLoopState() const
{
    switch (m_hpv_player->getLoopMode()) {
        case HPV_LOOPMODE_NONE:
            return OF_LOOP_NONE;
            break;
        case HPV_LOOPMODE_LOOP:
            return OF_LOOP_NORMAL;
            break;
        case HPV_LOOPMODE_PALINDROME:
            return OF_LOOP_PALINDROME;
            break;
            
        default:
            return OF_LOOP_NONE;
            break;
    }
}

int64_t ofxHPVPlayer::getLoopIn() const
{
    return m_hpv_player->getLoopIn();
}

int64_t ofxHPVPlayer::getLoopOut() const
{
    return m_hpv_player->getLoopOut();
}

void ofxHPVPlayer::firstFrame()
{
    this->seekToFrame(m_hpv_player->getLoopIn());
}

void ofxHPVPlayer::nextFrame()
{
    int64_t cur_frame = this->getCurrentFrame();
    this->seekToFrame((++cur_frame > m_hpv_player->getLoopOut()) ? m_hpv_player->getLoopIn() : cur_frame);
}

void ofxHPVPlayer::previousFrame()
{
    int64_t cur_frame = this->getCurrentFrame();
    this->seekToFrame((--cur_frame < m_hpv_player->getLoopIn()) ? m_hpv_player->getLoopOut() : cur_frame);
}

void ofxHPVPlayer::lastFrame()
{
    this->seekToFrame(m_hpv_player->getLoopOut());
}

// Closes the video file and the file stream
void ofxHPVPlayer::close()
{
    m_hpv_player->close();
}

// Set loop in point, this is the new 'start' point
void ofxHPVPlayer::setLoopIn(uint64_t loopIn)
{
    m_hpv_player->setLoopInPoint(loopIn);
}

// Set loop out point, this is the new 'end' point
void ofxHPVPlayer::setLoopOut(uint64_t loopOut)
{
    m_hpv_player->setLoopOutPoint(loopOut);
}

// Report back the video frame rate
int ofxHPVPlayer::getFrameRate() const
{
    return m_hpv_player->getFrameRate();
}

// Report back the video compression type
int ofxHPVPlayer::getCompressionType() const
{
    return static_cast<int>(m_hpv_player->getCompressionType());
}

// set the playback direction
void ofxHPVPlayer::setPlayDirection(bool direction)
{
    m_hpv_player->setPlayDirection(direction);
}

// seek to normalized pos in range [0.,1.]
void ofxHPVPlayer::seekToPos(double pos)
{
    m_hpv_player->seek(pos);
}

// seek to frame number
void ofxHPVPlayer::seekToFrame(int64_t frame)
{
    m_hpv_player->seek(frame);
}

bool ofxHPVPlayer::needsDoubleBuffering() const
{
    return RendererSingleton()->needsBuffering(m_hpv_player->getID());
}

void ofxHPVPlayer::setDoubleBuffered(bool bDoubleBuffer)
{
    if (bDoubleBuffer)
    {
        RendererSingleton()->setRenderState(m_hpv_player->getID(), HPV::HPVRenderState::STATE_BUFFER);
    }
    else
    {
        RendererSingleton()->setRenderState(m_hpv_player->getID(), HPV::HPVRenderState::STATE_BLIT);
    }
}

// get pointer to stats struct report
HPVDecodeStats * ofxHPVPlayer::getDecodeStatsPtr() const
{
    return &m_hpv_player->_decode_stats;
}

std::string ofxHPVPlayer::getFileSummary()
{
    return m_hpv_player->getFileSummary();
}

ofPixelFormat ofxHPVPlayer::getPixelFormat() const
{
    return OF_PIXELS_RGBA;
}

void ofxHPVPlayer::draw(float x, float y, float width, float height)
{
    if (m_texture.isAllocated())
    {
        if (m_hpv_player->getCompressionType() == HPVCompressionType::HPV_TYPE_SCALED_DXT5_CoCg_Y)
        {
            m_shader.begin();
            m_shader.bindDefaults();
        }
    
        m_texture.draw(x, y, width, height);

        if (m_hpv_player->getCompressionType() == HPVCompressionType::HPV_TYPE_SCALED_DXT5_CoCg_Y)
        {
            m_shader.end();
        }
    }
}