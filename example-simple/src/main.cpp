#include "ofMain.h"
#include "ofApp.h"

int main( )
{
    ofGLWindowSettings settings;
    settings.setSize(1024, 512);
    settings.windowMode = OF_WINDOW;
    settings.setGLVersion(4, 1);
    
    ofCreateWindow(settings);
    
    ofRunApp(new ofApp());
}
