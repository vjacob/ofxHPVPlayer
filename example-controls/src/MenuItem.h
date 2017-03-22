#ifndef MenuItem_h
#define MenuItem_h

#pragma once

#include "ofMain.h"

class MenuItem
{
public:
    MenuItem(std::string path, float _x, float _y, std::string _name)
    {
        name = _name;
        
        if (ofLoadImage(tex, path))
        {
            if (tex.isAllocated())
            {
                draw_pos.x = _x;
                draw_pos.y = _y;
                float rect_x = draw_pos.x - (tex.getWidth() / 2);
                float rect_y = draw_pos.y - (tex.getHeight() / 2);
                hit_zone = ofRectangle(rect_x, rect_y, tex.getWidth(), tex.getHeight());
                tex.setAnchorPercent(0.5, 0.5);
            }
        }
        
        b_hovered = false;
        b_selected = false;
    }
    
    ~MenuItem()
    {
        if (tex.isAllocated())
        {
            tex.clear();
        }
    }
    
    void draw()
    {
        if (!tex.isAllocated())
            return;
        
        ofPushStyle();
        if (b_hovered)  ofSetColor(ofColor::lightGrey);
        else if (b_selected) ofSetColor(ofColor::orange);
        else ofSetColor(ofColor::white);
        tex.draw(draw_pos);
        ofPopStyle();
    }
    
    bool inside(ofVec2f hit)
    {
        bool _hit = hit_zone.inside(hit);
        
        if (_hit)
        {
            if (b_selected) return _hit;
            else
            {
                b_hovered = true;
                return _hit;
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
        b_selected = !b_selected;
        b_hovered = !b_selected;
    }
    
    void unselect()
    {
        b_selected = false;
        b_hovered = false;
    }
    
    std::string& getName()
    {
        return name;
    }
    
    
private:
    ofVec2f draw_pos;
    ofTexture tex;
    ofRectangle hit_zone;
    std::string name;
    bool b_hovered;
    bool b_selected;
};

#endif /* MenuItem_h */
