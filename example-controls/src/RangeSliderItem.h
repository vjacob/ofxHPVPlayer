#ifndef RangeSliderItem_h
#define RangeSliderItem_h

#pragma once

#include "ofMain.h"

class RangeSliderItem
{
public:
    RangeSliderItem(float _x, float _y, float _width, float _height, std::string _name)
    {
        name = _name;
        
        draw_pos.x = _x;
        draw_pos.y = _y;
        float rect_x = draw_pos.x - (_width / 2);
        float rect_y = draw_pos.y - (_height / 2);
        hit_zone = ofRectangle(rect_x, rect_y, _width, _height);
        
        b_selected = false;
        b_hovered = false;
        
        range_in = 0.f;
        range_out = 1.f;
    }
    
    RangeSliderItem()
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
        ofDrawLine(line_x+cur_pos, line_y-10, line_x+cur_pos, line_y+10);
        
        if (range_in > 0.0f || range_out < 1.0f)
        {
            /* Loop range */
            ofSetColor(255,174,0,100);
            float range_start = line_x + (hit_zone.width * range_in);
            float range_end = hit_zone.width * (range_out-range_in);
            ofDrawRectangle(range_start, line_y-10, range_end, 20);
        }

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
    
    void setPlayhead(float _cur_pos)
    {
        cur_pos = _cur_pos*hit_zone.width;
    }
    
    void mouseDown(const ofVec2f& _cursor)
    {
        cur_pos = (_cursor.x - (hit_zone.x + hit_zone.width * range_in)) / ((range_out - range_in) * hit_zone.width);
    }
    
    void setRangeIn(float _in)
    {
        range_in = (_in - hit_zone.x) / hit_zone.width;
        
        if (range_in > range_out)
        {
            range_in = MAX(0.0f,range_out-0.1f);
        }
    }
    
    void setRangeOut(float _out)
    {
        range_out = (_out - hit_zone.x) / hit_zone.width;
        
        if (range_out < range_in)
        {
            range_out = MIN(1.0f,range_in+0.1f);
        }
    }
    
    void clearRange()
    {
        range_in = 0.f;
        range_out = 1.f;
    }
    
    float getValue()
    {
        return cur_pos;
    }
    
    ofVec2f getSelectedRange()
    {
        return ofVec2f(range_in, range_out);
    }
    
    ofRectangle& getRect()
    {
        return hit_zone;
    }
    
private:
    ofVec2f draw_pos;
    ofRectangle hit_zone;
    float cur_pos;
    float range_in;
    float range_out;
    std::string name;
    bool b_hovered;
    bool b_selected;
};


#endif /* RangeSliderItem_h */
