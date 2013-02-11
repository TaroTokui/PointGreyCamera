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
    
private:
    PGRCamera* pgcamera;
    ofxCvGrayscaleImage grayImage, bgImage, fsImage, srcImage;
    ofxCvContourFinder contourFinder;
    
    ofxControlPanel gui;
    
    CvBox2D32f minRect;
    
    // ピクセルデータ保存用
    unsigned char *srcPixels;    // 背景画像
    unsigned char *bgPixels;    // 背景画像
    unsigned char *fsPixels;    // 前景画像
    
    int margin; // 背景と前景の差
    bool background;
    
};
