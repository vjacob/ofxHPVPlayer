#pragma once

#include <stdio.h>
#include <vector>
#include <string>

#include "Log.h"
#include "HPVHeader.h"
#include "HPVManager.h"
#include "HPVPlayer.h"
#include "HPVRenderBridge.h"

#include "ofTypes.h"

using namespace HPV;

class ofxHPVPlayer
{
public:
    ofxHPVPlayer();
    ~ofxHPVPlayer();
    
    void init(HPVPlayerRef internal_hpv_player);

    bool                load(string name);
    bool                loadAsync(string name);
    
    void                play();
    void                stop();
    void                close();
    
    /* 
     * NOT SUPPORTED
     * bool                isFrameNew() const;
     */
    
    ofTexture *         getTexturePtr();
    float               getWidth() const;
    float               getHeight() const;
    float               getPosition() const;
    float               getSpeed() const;
    float               getDuration() const;
    bool                getIsMovieDone() const;
    int                 getCurrentFrame() const;
    int                 getTotalNumFrames() const;
    ofLoopType          getLoopState() const;
    int64_t             getLoopIn() const;
    int64_t             getLoopOut() const;
    int                 getFrameRate() const;
    int                 getCompressionType() const;
    
    ofPixelFormat       getPixelFormat() const;
    
    bool                needsDoubleBuffering() const;
    
    /* 
     * NOT SUPPORTED, HPV is purely GPU oriented!
     * ofPixels&           getPixels();
     * const ofPixels&     getPixels() const;
     */
    
    bool                isPaused() const;
    bool                isLoaded() const;
    bool                isPlaying() const;
    bool                isInitialized() const{ return isLoaded(); }
   
    void                setPaused(bool bPause);
    void                setPosition(float pct);
    void                setVolume(float volume);
    void                setLoopState(ofLoopType state);
    void                setSpeed(float speed);
    void                setFrame(int frame);
    void                setLoopIn(uint64_t loopIn);
    void                setLoopOut(uint64_t loopOut);
    void                setPlayDirection(bool direction);
    void                seekToPos(double pos, bool sync = true);
    void                seekToFrame(int64_t frame, bool sync = true);
    
    void                setDoubleBuffered(bool bDoubleBuffer);
     
    void                firstFrame();
    void                nextFrame();
    void                previousFrame();
    void                lastFrame();
   
    HPVDecodeStats *    getDecodeStatsPtr() const;
    
    std::string         getFileSummary();
    
    void                draw(float x, float y, float width, float height);
    void                drawSubsection(float x, float y, float w, float h, float sx, float sy, float sw, float sh);

private:
    ofShader            m_shader;
    ofTexture           m_texture;
    HPVPlayerRef        m_hpv_player;
};
