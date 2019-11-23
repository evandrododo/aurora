#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){

    ofSetupOpenGL(1366,768, OF_FULLSCREEN);
    ofRunApp( new ofApp() );
}
