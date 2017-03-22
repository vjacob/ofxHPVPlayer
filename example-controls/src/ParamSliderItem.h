#ifndef ParamSliderItem_h
#define ParamSliderItem_h

#pragma once

#include "ofMain.h"

class ParamSliderItem
{
public:
    ParamSliderItem(float _x, float _y, float _width, float _height, float _min, float _max, std::string _name)
    {
        name = _name;
        
        draw_pos.x = _x;
        draw_pos.y = _y;
        float rect_x = draw_pos.x - (_width / 2);
        float rect_y = draw_pos.y - (_height / 2);
        hit_zone = ofRectangle(rect_x, rect_y, _width, _height);
        
        min_val = _min;
        max_val = _max;
        
        b_selected = false;
        b_hovered = false;
        
        reset();
    }
    
    ParamSliderItem()
    {
        
    }
    
    void draw()
    {
        ofPushStyle();
        
        if (b_hovered) ofSetColor(ofColor::lightGrey);
        else ofSetColor(ofColor::white);
        
        float line_x = hit_zone.x;
        float line_y = hit_zone.y + (hit_zone.height / 2);
        
        /* Main transport line */
        ofDrawLine(line_x, line_y, line_x+hit_zone.width, line_y);
        
        /* Cursor */
        ofSetLineWidth(3.0f);
        ofDrawLine(line_x+(cur_pos*hit_zone.width), line_y-10, line_x+(cur_pos*hit_zone.width), line_y+10);
        
        ofPopStyle();
    }
    
    bool inside(const ofVec2f& hit)
    {
        if (hit_zone.inside(hit))
        {
            if (b_selected) return true;
            else
            {
                b_hovered = true;
                return true;
            }
        }
        else
        {
            b_hovered = false;
            return false;
        }
    }
    
    void select()
    {
        
    }
    
    void unselect()
    {
        
    }
    
    std::string& getName()
    {
        
    }
    
    void mouseDown(const ofVec2f& _mouse)
    {
        cur_pos = (_mouse.x - hit_zone.x) / hit_zone.width;
        cur_val = ofMap(cur_pos, 0.0f, 1.0f, min_val, max_val);
    }
    
    float getValue()
    {
        return cur_val;
    }
    
    ofRectangle& getRect()
    {
        return hit_zone;
    }
    
    void reset()
    {
        cur_val = 1.0f;
        cur_pos = ofMap(cur_val, min_val, max_val, 0.0f, 1.0f);
    }
    
private:
    ofVec2f draw_pos;
    ofRectangle hit_zone;
    float cur_pos;
    float cur_val;
    float min_val;
    float max_val;
    std::string name;
    bool b_hovered;
    bool b_selected;
};

#endif /* ParamSlider_h */
