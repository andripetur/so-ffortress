#include "ofApp.h"
#include "ofMain.h"
#include "ofxAppGLFWWindowMulti.h"

int main() {
    ofxAppGLFWWindowMulti * window = new ofxAppGLFWWindowMulti();

    ofSetupOpenGL(window, 1024, 768, OF_WINDOW);
	ofRunApp(new ofApp());
}
