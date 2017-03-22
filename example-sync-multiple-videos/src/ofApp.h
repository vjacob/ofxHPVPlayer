#pragma once

#include "ofMain.h"
#include "ofxHPVPlayer.h"

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void windowResized(int w, int h);
    
    ofxHPVPlayer top_left;
    ofxHPVPlayer top_right;
    ofxHPVPlayer bottom_left;
    ofxHPVPlayer bottom_right;
};
