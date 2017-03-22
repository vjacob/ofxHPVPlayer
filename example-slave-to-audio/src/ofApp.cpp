#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    HPV::InitHPVEngine();
    HPV::AddEventListener(this, &ofApp::onHPVEvent);

    hpvPlayer.init(HPV::NewPlayer());
    if (hpvPlayer.load("bbb_export.hpv"))
    {
        hpvPlayer.setLoopState(OF_LOOP_NORMAL);
        hpvPlayer.setPaused(true);
        
        if (hpvPlayer.getFrameRate() > 60)
        {
            ofSetVerticalSync(false);
            ofSetFrameRate(120);
        }
        else
        {
            ofSetVerticalSync(true);
        }
    }
    
    if (soundPlayer.load("bbb_export.wav"))
    {
        soundPlayer.play();
        soundPlayer.setLoop(true);
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
    hpvPlayer.seekToPos(soundPlayer.getPosition());
    HPV::Update();    
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(0);
    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    hpvPlayer.draw(0,0,ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::exit()
{
    HPV::DestroyHPVEngine();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'r')
    {
        soundPlayer.setPosition(0);
    }
    else if (key == 'f')
    {
        ofToggleFullscreen();
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
