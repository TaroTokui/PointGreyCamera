//
//  PGRCamera.h
//  ofxKinectExample
//
//  Created by tokui on 13/02/08.
//
//
#pragma once

#include "ofxOpenCv.h"
#include <dc1394/dc1394.h>
#include <dc1394/capture.h>

class PGRCamera{

public:
    PGRCamera();
    ~PGRCamera();
    
    void setup();
    void update();
    void draw();
    
    ofxCvGrayscaleImage getGrayImage();
    
    void chkErr(dc1394error_t pError);
    void exposure(float value);
    
private:
    unsigned char           *mCameraTexture;
    dc1394camera_t          *mCamera;
    ofxCvColorImage srcImage;
    ofxCvGrayscaleImage grayImage;
    
};
