#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    HPV::InitHPVEngine();
    
    hpvPlayer.init(HPV::NewPlayer());
    
    if (hpvPlayer.load("equi_export.hpv"))
    {
        hpvPlayer.setDoubleBuffered(true);
        hpvPlayer.setLoopState(OF_LOOP_NORMAL);
        hpvPlayer.play();
    }
    
    sphere = ofSpherePrimitive(static_cast<float>(ofGetWidth() / 2), 50);
    sphere.rotate(180, 0, 0, 1);
    
    b_draw_equi = false;

	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update()
{
    HPV::Update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    if (b_draw_equi)
    {
        hpvPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
    else
    {
        cam.begin();

        hpvPlayer.getTexturePtr()->bind();
        sphere.draw();
        hpvPlayer.getTexturePtr()->unbind();
        
        cam.end();
    }

	std::stringstream ss;
	ss  << "HDD: " << hpvPlayer.getDecodeStatsPtr()->hdd_read_time / 1e6 << "ms"
		<< std::endl
		<< "L4Z: " << hpvPlayer.getDecodeStatsPtr()->l4z_decode_time / 1e6 << "ms"
		<< std::endl
		<< "GPU: " << hpvPlayer.getDecodeStatsPtr()->gpu_upload_time / 1e6 << "ms"
		<< std::endl
		<< "TOT: " << (hpvPlayer.getDecodeStatsPtr()->hdd_read_time + hpvPlayer.getDecodeStatsPtr()->l4z_decode_time + hpvPlayer.getDecodeStatsPtr()->gpu_upload_time) / 1e6
		<< std::endl
		<< "FRAME: " << hpvPlayer.getCurrentFrame()
		<< std::endl
		<< "SPEED: " << hpvPlayer.getSpeed()
		<< std::endl
		<< "DURATION: " << hpvPlayer.getDuration()
		<< std::endl
		<< "DONE: " << hpvPlayer.getIsMovieDone();

	ofDrawBitmapString(ss.str(), 50, 100);
    
    ofDrawBitmapStringHighlight("Press 'e' to toggle between EQUIRECTANGULAR and PERSPECTIVE view", 50, 50);
}

//--------------------------------------------------------------
void ofApp::exit()
{
    HPV::DestroyHPVEngine();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if (key == 'f')
    {
        ofToggleFullscreen();
    }
    else if (key == 'e')
    {
        b_draw_equi = !b_draw_equi;
    }
}