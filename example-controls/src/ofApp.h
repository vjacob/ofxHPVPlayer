#pragma once

#include "ofMain.h"
#include "ofxHPVPlayer.h"
#include "MenuItem.h"
#include "RangeSliderItem.h"
#include "ParamSliderItem.h"
#include "ofTrueTypeFontExt.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();
	void exit();
    
    void onHPVEvent(const HPVEvent& event);
    
    void setupUI();
    
	void keyPressed(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void windowResized(int w, int h);
    
	ofxHPVPlayer hpvPlayer;
    
    std::map<std::string, MenuItem *> ui_items;
    RangeSliderItem * range_slider;
    ParamSliderItem * speed_slider;
    
    bool b_draw_stats;
    bool b_draw_gui;
    
    ofTrueTypeFontExt gui_font;
};
