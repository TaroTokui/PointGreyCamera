#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

#include <dc1394/dc1394.h>
#include <dc1394/capture.h>

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
private:
    void                    check_error(dc1394error_t pError);
    unsigned char           *mCameraTexture;
    dc1394camera_t          *mCamera;
    ofxCvColorImage srcImage;
    
};
