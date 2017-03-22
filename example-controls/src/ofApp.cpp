#include "ofApp.h"

static int prev_frame = 0;
static bool db = false;

//--------------------------------------------------------------
void ofApp::setup()
{
    HPV::InitHPVEngine();
    HPV::AddEventListener(this, &ofApp::onHPVEvent);

    b_draw_stats = b_draw_gui = true;
    hpvPlayer.init(HPV::NewPlayer());
    
    if (hpvPlayer.load("bbb_export.hpv"))
    {
        hpvPlayer.setLoopState(OF_LOOP_NORMAL);
        hpvPlayer.setDoubleBuffered(db);
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
    
    this->setupUI();
}

//--------------------------------------------------------------
void ofApp::update()
{
    HPV::Update();
    
    range_slider->setPlayhead(hpvPlayer.getCurrentFrame()/(float)hpvPlayer.getTotalNumFrames());
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(0);
    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    ofSetColor(255);
    hpvPlayer.draw(0,0,ofGetWidth(), ofGetHeight());
    
    if (b_draw_gui)
    {
        float y = ofGetHeight() - 150;
        
        ofPushStyle();
        ofSetColor(125);
        ofDrawLine(0, y, ofGetWidth(), y);
        
        ofSetColor(0, 0, 0, 100);
        ofDrawRectangle(0, y, ofGetWidth(), y);
        ofPopStyle();
        
        for (auto& item : ui_items)
        {
            item.second->draw();
        }
        
        speed_slider->draw();
        range_slider->draw();
        
        gui_font.drawStringShadowed("speed\n(middle mouse to reset)", (ofGetWidth() / 2) + 310, speed_slider->getRect().y + (speed_slider->getRect().height/2) + 3, ofColor(225));
        gui_font.drawStringShadowed("scrub + range\npress 'b' and 'n' while clicking on the\nrange to define loop points\n(middle mouse to reset)", (ofGetWidth() / 2) + 310, range_slider->getRect().y + (range_slider->getRect().height/2) + 3, ofColor(225));

        gui_font.drawStringShadowed(hpvPlayer.getFileSummary(), (ofGetWidth() / 2) - 250, range_slider->getRect().y + 60, ofColor(225));
    }
    
    if (b_draw_stats)
    {
        std::stringstream ss;
        ss << "DOUBLE BUFFER: " << (db ? "ON" : "OFF")
        << " ('d' toggles)"
        << std::endl
        << "HDD: " << hpvPlayer.getDecodeStatsPtr()->hdd_read_time / 1e6 << "ms"
        << std::endl
        << "L4Z: " << hpvPlayer.getDecodeStatsPtr()->l4z_decode_time / 1e6 << "ms"
        << std::endl
        << "GPU: " << hpvPlayer.getDecodeStatsPtr()->gpu_upload_time / 1e6 << "ms"
        << std::endl
        << "TOT: " << (hpvPlayer.getDecodeStatsPtr()->hdd_read_time+hpvPlayer.getDecodeStatsPtr()->l4z_decode_time+hpvPlayer.getDecodeStatsPtr()->gpu_upload_time) / 1e6
        << std::endl
        << "FRAME: " << hpvPlayer.getCurrentFrame()
        << std::endl
        << "SPEED: " << hpvPlayer.getSpeed()
        << std::endl
        << "DURATION: " << hpvPlayer.getDuration()
        << std::endl
        << "DONE: " << hpvPlayer.getIsMovieDone();
        
        gui_font.drawStringShadowed(ss.str(), ofVec2f(25,ofGetHeight()-130));
    }
}

//--------------------------------------------------------------
void ofApp::exit()
{
    HPV::DestroyHPVEngine();
}

//--------------------------------------------------------------
void ofApp::setupUI()
{
    float half_width = ofGetWidth() / 2;
    MenuItem * play = new MenuItem("ui/button_play.png", half_width - 270, ofGetHeight()-100, "PLAY");
    MenuItem * pause = new MenuItem("ui/button_pause.png", half_width - 210, ofGetHeight()-100, "PAUSE");
    MenuItem * stop = new MenuItem("ui/button_stop.png", half_width - 150, ofGetHeight()-100, "STOP");
    MenuItem * single_play = new MenuItem("ui/button_single.png", half_width - 70, ofGetHeight()-100, "SINGLE");
    MenuItem * loop_normal = new MenuItem("ui/button_loop.png", half_width + 30, ofGetHeight()-100, "LOOP");
    MenuItem * loop_palindrome = new MenuItem("ui/button_palindrome.png", half_width + 130, ofGetHeight()-100, "PALINDROME");
    
    ui_items["PLAY"] = play;
    ui_items["STOP"] = stop;
    ui_items["PAUSE"] = pause;
    ui_items["SINGLE"] = single_play;
    ui_items["LOOP"] = loop_normal;
    ui_items["PALINDROME"] = loop_palindrome;

    range_slider = new RangeSliderItem(ofGetWidth()/2, ofGetHeight()-50, 600, 50, "RANGE_SLIDER");
    speed_slider = new ParamSliderItem(half_width + 240, ofGetHeight()-100, 100, 50, -6.0f, 6.0f, "SPEED_SLIDER");
    
    gui_font.load("ui/DIN.otf", 9);
    
    play->select();
    loop_normal->select();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'f')
    {
        ofToggleFullscreen();
    }
    else if (key == 's')
    {
        b_draw_stats = !b_draw_stats;
    }
    else if (key == 'g')
    {
        b_draw_gui = !b_draw_gui;
    }
    else if (key == 'r')
    {
        hpvPlayer.firstFrame();
        if (!hpvPlayer.isPlaying()) hpvPlayer.play();
    }
    else if (key == '+')
    {
        hpvPlayer.nextFrame();
    }
    else if (key == '-')
    {
        hpvPlayer.previousFrame();
    }
    else if (key == 'd')
    {
        db = !db;
        hpvPlayer.setDoubleBuffered(db);
    }
}

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
            ui_items["PLAY"]->unselect();
            ui_items["PAUSE"]->unselect();
            ui_items["STOP"]->select();
            break;
        case HPV::HPVEventType::HPV_EVENT_LOOP:
            cout << "'" << event.player->getFilename() << "': loop event" << endl;
            break;
        case HPV::HPVEventType::HPV_EVENT_NUM_TYPES:
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
    ofVec2f mouse(x,y);
    for (auto& item : ui_items)
    {
        item.second->inside(mouse);
    }
    
    range_slider->inside(mouse);
    speed_slider->inside(mouse);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    ofVec2f mouse(x,y);
    
    if (range_slider->inside(mouse))
    {
       	range_slider->mouseDown(mouse);
        
        int frame = hpvPlayer.getLoopIn() + (range_slider->getValue() * (hpvPlayer.getLoopOut()-hpvPlayer.getLoopIn()));
        
        if (frame != prev_frame)
        {
            hpvPlayer.seekToFrame(frame);
            prev_frame = frame;
        }
    }
    
    if (speed_slider->inside(mouse))
    {
        speed_slider->mouseDown(mouse);
        
        if (ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE) || ofGetKeyPressed('c'))
        {
            speed_slider->reset();
        }
        
        hpvPlayer.setSpeed(speed_slider->getValue());
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    ofVec2f mouse(x,y);
    
    for (auto& item : ui_items)
    {
        MenuItem * menu_item = item.second;
        
        if (menu_item->inside(mouse))
        {
            menu_item->select();
            
            if (menu_item->getName() == "PLAY")
            {
                if (hpvPlayer.isPaused())
                {
                    hpvPlayer.setPaused(false);
                }
                else
                {
                    hpvPlayer.play();
                }
                
                ui_items["PAUSE"]->unselect();
                ui_items["STOP"]->unselect();
            }
            else if (menu_item->getName() == "STOP")
            {
                hpvPlayer.stop();
                
                ui_items["PLAY"]->unselect();
                ui_items["PAUSE"]->unselect();
            }
            else if (menu_item->getName() == "PAUSE")
            {
                hpvPlayer.setPaused(true);
                
                ui_items["PLAY"]->unselect();
                ui_items["STOP"]->unselect();
            }
            else if (menu_item->getName() == "SINGLE")
            {
                hpvPlayer.setLoopState(OF_LOOP_NONE);
                
                ui_items["LOOP"]->unselect();
                ui_items["PALINDROME"]->unselect();
            }
            else if (menu_item->getName() == "LOOP")
            {
                hpvPlayer.setLoopState(OF_LOOP_NORMAL);
                
                ui_items["SINGLE"]->unselect();
                ui_items["PALINDROME"]->unselect();
            }
            else if (menu_item->getName() == "PALINDROME")
            {
                hpvPlayer.setLoopState(OF_LOOP_PALINDROME);
                
                ui_items["SINGLE"]->unselect();
                ui_items["LOOP"]->unselect();
            }
            
            return;
        }
    }
    
    if (range_slider->inside(mouse))
    {
        range_slider->mouseDown(mouse);
        
        bool b_adjust_range = false;
        if (ofGetKeyPressed('b'))
        {
            range_slider->setRangeIn(mouse.x);
            b_adjust_range = true;
        }
        else if (ofGetKeyPressed('n'))
        {
            range_slider->setRangeOut(mouse.x);
            b_adjust_range = true;
        }
        else if (ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE) || ofGetKeyPressed('c'))
        {
            range_slider->clearRange();
            b_adjust_range = true;
        }
        
        if (b_adjust_range)
        {
            ofVec2f range = range_slider->getSelectedRange();
            range *= hpvPlayer.getTotalNumFrames();
        
            hpvPlayer.setLoopIn(range.x);
            hpvPlayer.setLoopOut(range.y);
        }
        else
        {
            int frame = hpvPlayer.getLoopIn() + (range_slider->getValue() * (hpvPlayer.getLoopOut()-hpvPlayer.getLoopIn()));
            
            if (frame != prev_frame)
            {
                hpvPlayer.seekToFrame(frame);
                prev_frame = frame;
            }
        }
        
        return;
    }
    
    if (speed_slider->inside(mouse))
    {
        speed_slider->mouseDown(mouse);
        
        if (ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE) || ofGetKeyPressed('c'))
        {
            speed_slider->reset();
        }
        
        hpvPlayer.setSpeed(speed_slider->getValue());
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    for (auto& item : ui_items)
    {
        delete(item.second);
        item.second = nullptr;
    }
    
    ui_items.clear();
    
    delete(range_slider);
    range_slider = nullptr;
    delete(speed_slider);
    speed_slider = nullptr;
    
    this->setupUI();
}
