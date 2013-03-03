#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "PGRCamera.h"
#include "ofxControlPanel.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
    void fitBox( vector <ofPoint>  & contour, CvBox2D32f & box);
    
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    
    void setupArduino(const int & version);
    void digitalPinChanged(const int & pinNum);
    void analogPinChanged(const int & pinNum);
	void updateArduino();
    
    ofArduino ard;
	bool		bSetupArduino;			// flag variable for setting up arduino once
    
    string buttonState;
    string potValue;
    
    ofFbo fbo;
    // 2013/03/03
//    unsigned char *fboPixels;    // 背景画像
//    ofImage fboImage;
    
    void drawFbo(); // draws some test graphics into the two fbos
    
private:
    PGRCamera* pgcamera;
    ofxCvGrayscaleImage grayImage, bgImage, fsImage, srcImage;
    ofxCvContourFinder contourFinder;
    
    ofxControlPanel gui;
    
    CvBox2D32f minRect, preMinRect;
    
    // ピクセルデータ保存用
    unsigned char *srcPixels;    // 背景画像
    unsigned char *bgPixels;    // 背景画像
    unsigned char *fsPixels;    // 前景画像
    
    int margin; // 背景と前景の差
    bool background;
    
    ofImage texImage, texImageR;
    
};
