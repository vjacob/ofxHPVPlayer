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

    void keyReleased(int key);
    
    ofxHPVPlayer hpvPlayer;
    ofSpherePrimitive sphere;
    ofEasyCam cam;
    
    bool b_draw_equi;
};
