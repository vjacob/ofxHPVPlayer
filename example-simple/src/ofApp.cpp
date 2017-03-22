#include "ofApp.h"

static bool pauseState = false;

//--------------------------------------------------------------
void ofApp::setup()
{
    /* Init HPV Engine */
    HPV::InitHPVEngine();
    
    /* Create resources for new player */
    hpvPlayer.init(HPV::NewPlayer());
    
    /* Try to load file and start playback */
    if (hpvPlayer.load("bbb_export.hpv"))
    {
        hpvPlayer.setLoopState(OF_LOOP_NORMAL);
        hpvPlayer.play();
    }
    
    /* Enable vertical sync, if file has fps > 60, you might have to set to false */
    ofSetVerticalSync(true);
    
    /* Alternatively, if you experience playback stutter, try to toggle double-buffering true/false
     * Default: OFF
     *
     * hpvPlayer.setDoubleBuffered(true);
     */
}

//--------------------------------------------------------------
void ofApp::update()
{
    /* Update happens on global level for all active players by HPV Manager */
    HPV::Update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    /* Draw the texture fullscreen */
    ofBackground(0);
    hpvPlayer.draw(0,0,ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::exit()
{
    /* Cleanup and destroy HPV Engine upon exit */
    HPV::DestroyHPVEngine();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'f')
    {
        ofToggleFullscreen();
    }
    else if (key == 'p')
    {
        hpvPlayer.play();
    }
    else if (key == ' ')
    {
        pauseState = !pauseState;
        hpvPlayer.setPaused(pauseState);
    }
    else if (key == 's')
    {
        hpvPlayer.stop();
    }
}

//--------------------------------------------------------------
void ofApp::onHPVEvent(const HPVEvent& event)
{
    switch (event.type)
    {
        case HPV::HPVEventType::HPV_EVENT_PLAY:
            cout << "'" << event.player->getFilename() << "': play event" << endl;
            break;
        case HPV::HPVEventType::HPV_EVENT_PAUSE:
            cout << "'" << event.player->getFilename() << "': pause event" << endl;
            break;
        case HPV::HPVEventType::HPV_EVENT_RESUME:
            cout << "'" << event.player->getFilename() << "': resume event" << endl;
            break;
        case HPV::HPVEventType::HPV_EVENT_STOP:
            cout << "'" << event.player->getFilename() << "': stop event" << endl;
            break;
        case HPV::HPVEventType::HPV_EVENT_LOOP:
            cout << "'" << event.player->getFilename() << "': loop event" << endl;
            break;
        case HPV::HPVEventType::HPV_EVENT_NUM_TYPES:
        default:
            break;
    }
}