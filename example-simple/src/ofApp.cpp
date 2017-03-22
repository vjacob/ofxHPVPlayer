#include "ofApp.h"

static bool pauseState = false;

//--------------------------------------------------------------
void ofApp::setup()
{
    HPV::InitHPVEngine();
    HPV::AddEventListener(this, &ofApp::onHPVEvent);

    hpvPlayer.init(HPV::NewPlayer());
    
    if (hpvPlayer.load("bbb_export.hpv"))
    {
        hpvPlayer.setDoubleBuffered(true);
        hpvPlayer.setLoopState(OF_LOOP_NORMAL);
        hpvPlayer.play();

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
}

//--------------------------------------------------------------
void ofApp::update()
{
    HPV::Update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    hpvPlayer.draw(0,0,ofGetWidth(), ofGetHeight());
}

void ofApp::exit()
{
    HPV::DestroyHPVEngine();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
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