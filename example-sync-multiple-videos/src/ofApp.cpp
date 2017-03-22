#include "ofApp.h"

int SIDE_WIDTH;
int SIDE_HEIGHT;

static int64_t prev_frame = 0;
static int64_t cur_frame = 0;
static uint8_t offset = 1;

//--------------------------------------------------------------
void ofApp::setup()
{
    HPV::InitHPVEngine();
    
    top_left.init(HPV::NewPlayer());
    top_left.load("top_left.hpv");
    top_left.setLoopState(OF_LOOP_NORMAL);
    
    top_right.init(HPV::NewPlayer());
    top_right.load("top_right.hpv");
    top_right.setLoopState(OF_LOOP_NORMAL);
    
    bottom_left.init(HPV::NewPlayer());
    bottom_left.load("bottom_left.hpv");
    bottom_left.setLoopState(OF_LOOP_NORMAL);
    
    bottom_right.init(HPV::NewPlayer());
    bottom_right.load("bottom_right.hpv");
    bottom_right.setLoopState(OF_LOOP_NORMAL);

    top_left.play();
    top_right.play();
    bottom_left.play();
    bottom_right.play();
    
    top_left.setPaused(true);
    top_right.setPaused(true);
    bottom_left.setPaused(true);
    bottom_right.setPaused(true);
    
    ofSetVerticalSync(false);
    ofSetFrameRate(120);
    
    SIDE_WIDTH = ofGetWidth() / 2;
    SIDE_HEIGHT = ofGetHeight() / 2;
}

//--------------------------------------------------------------
void ofApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    
    if (cur_frame != prev_frame)
    {
        top_left.seekToFrame(cur_frame);
        top_right.seekToFrame(cur_frame);
        bottom_left.seekToFrame(cur_frame);
        bottom_right.seekToFrame(cur_frame);
        
        prev_frame = cur_frame;
    }
    
    cur_frame++;
    
    if (cur_frame >= top_left.getTotalNumFrames())
    {
        cur_frame = 0;
    }
    
    HPV::Update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(255);
    
    top_left.draw(0, 0, SIDE_WIDTH-offset, SIDE_HEIGHT-offset);
    top_right.draw(SIDE_WIDTH+offset, 0, SIDE_WIDTH-offset, SIDE_HEIGHT-offset);
    bottom_left.draw(0, SIDE_HEIGHT+offset, SIDE_WIDTH-offset, SIDE_HEIGHT-offset);
    bottom_right.draw(SIDE_WIDTH+offset, SIDE_HEIGHT+offset, SIDE_WIDTH-offset, SIDE_HEIGHT-offset);
    
    ofDrawBitmapString(ofToString(top_left.getCurrentFrame()), SIDE_WIDTH-30, SIDE_HEIGHT-10);
    ofDrawBitmapString(ofToString(top_right.getCurrentFrame()), SIDE_WIDTH+10, SIDE_HEIGHT-10);
    ofDrawBitmapString(ofToString(bottom_left.getCurrentFrame()), SIDE_WIDTH-30, SIDE_HEIGHT+14);
    ofDrawBitmapString(ofToString(bottom_right.getCurrentFrame()), SIDE_WIDTH+10, SIDE_HEIGHT+14);
    
    ofDrawBitmapStringHighlight("Press 'g' to toggle grid mode, showing each video's individual frame", 50, 50);
}

//--------------------------------------------------------------
void ofApp::exit()
{
    HPV::DestroyHPVEngine();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'a')
    {
        cur_frame++;
    }
    else if (key == 'p')
    {
        top_left.setPaused(true);
        top_right.setPaused(true);
        bottom_left.setPaused(true);
        bottom_right.setPaused(true);
    }
    else if (key == 'f')
    {
        ofToggleFullscreen();
    }
    else if (key == 'g')
    {
        (offset > 0) ? (offset = 0) : (offset = 1);
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

    SIDE_WIDTH = ofGetWidth() / 2;
    SIDE_HEIGHT = ofGetHeight() / 2;
}
