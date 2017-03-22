#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings settings;
    settings.width = 1024;
    settings.height = 512;
    settings.windowMode = OF_WINDOW;
    settings.setGLVersion(4, 1);
    
    ofCreateWindow(settings);
	//ofSetupOpenGL(1024,512,OF_WINDOW);			// <-------- setup the GL context
    //shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    //shared_ptr<ofApp> mainApp(new ofApp);
    //ofRunApp(mainWindow, mainApp);
    //ofRunMainLoop();

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
