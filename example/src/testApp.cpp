#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofBackground(0);
    ofSetFullscreen(true);
	ofSetFrameRate(60);
    
    pgcamera = new PGRCamera();
    pgcamera->setup();
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    gui.setup("Control Panel", 0, 0, ofGetWidth()/1.4, ofGetHeight()/1.1);
    gui.addPanel("Setting", 3);
    gui.setWhichColumn(0);
    
    gui.addSlider("Threshold", "threshold", 200, 0, 255, true);
    gui.addSlider("MinArea", "minArea", 1000, 0, 10000, true);
    gui.addSlider("MaxArea", "maxArea", 10000, 0, 10000, true);
    gui.addSlider("MaxBlob", "maxBlob", 10, 0, 50, true);
    gui.addSlider("Margin", "margin", 50, 0, 255, true);
    gui.addToggle("dynamicDiffBG", "background", true);
    gui.addToggle("Reset", "reset", false);
    
    gui.setWhichColumn(1);
    gui.addDrawableRect("SrcImage", &srcImage, 320, 240);
    gui.addDrawableRect("BgImage", &bgImage, 320, 240);
    
    gui.setWhichColumn(2);
    gui.addDrawableRect("FrontSideImage", &fsImage,320, 240);
    gui.addDrawableRect("GrayImage", &grayImage, 320, 240);
    
    gui.loadSettings("controlPanelSettings.xml");

    // 画像領域を確保
    int width = pgcamera->getGrayImage().width;
    int height = pgcamera->getGrayImage().height;
    grayImage.allocate(width, height);
    bgImage.allocate(width, height);
    fsImage.allocate(width, height);
    srcImage.allocate(width, height);
    
    // ピクセルデータ保存用の領域を確保
    srcPixels = (unsigned char*)malloc(width * height);
    bgPixels = (unsigned char*)malloc(width * height);
    fsPixels = (unsigned char*)malloc(width * height);
    
    // bgPixelsを0で初期化
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) {
            bgPixels[i+j*width] = 255;
        }
    }
    
    texImage.loadImage("image007.jpeg");
    texImageR.loadImage("image007R.jpeg");
    
}

//--------------------------------------------------------------
void testApp::update() {
	
    pgcamera->update();
    grayImage = pgcamera->getGrayImage();
    srcImage = grayImage;

    fsPixels = grayImage.getPixels();
    
    int width = grayImage.width;
    int height = grayImage.height;
    
    cout << "w = " << width << " h = " << height << endl;
    
    if (gui.getValueB("reset")) {
        // bgPixelsを0で初期化
        for (int j=0; j<height; j++) {
            for (int i=0; i<width; i++) {
                bgPixels[i+j*width] = 255;
            }
        }
    }
    
    if(gui.getValueB("background")){
        for (int j=0; j<height; j++) {
            for (int i=0; i<width; i++) {
                // 前面の画素値が現在の背景より暗い場合は背景を更新
                if(fsPixels[i+j*width] < bgPixels[i+j*width] && fsPixels[i+j*width] != 0){
                    bgPixels[i+j*width] = fsPixels[i+j*width];
                }
            }
        }
        bgImage.setFromPixels(bgPixels, width, height);
    }else{
        // 背景より明るい領域を切り出す
        for (int j=0; j<height; j++) {
            for (int i=0; i<width; i++) {
                if( fsPixels[i+j*width] < bgPixels[i+j*width] + gui.getValueI("margin")){
                    fsPixels[i+j*width] = 0;
                }
            }
        }
        fsImage.setFromPixels(fsPixels, width, height);
        // ごま塩ノイズの除去
//        fsImage.erode_3x3();
//        fsImage.dilate_3x3();
    }
    
    // update the cv images
    bgImage.flagImageChanged();
    fsImage.flagImageChanged();
    
    grayImage.threshold(gui.getValueI("threshold"));
    contourFinder.findContours(grayImage, gui.getValueI("minArea"), gui.getValueI("maxArea"), gui.getValueI("maxBlob"), false);
	gui.update();
    
}

//--------------------------------------------------------------
void testApp::draw() {
	
    ofSetRectMode(OF_RECTMODE_CENTER);
	ofSetColor(255);
	
    grayImage.draw(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth(), ofGetHeight());
    
    ofSetColor(0, 0, 255);
    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    for (int i=0; i < contourFinder.nBlobs; i++) {
        fitBox(contourFinder.blobs.at(i).pts, minRect);

        ofPushMatrix();
        int tmpX = minRect.center.x * ofGetWidth() / grayImage.getWidth();
        int tmpY = minRect.center.y * ofGetHeight() / grayImage.getHeight();
        ofTranslate(tmpX, tmpY);
        ofRotateZ(minRect.angle);
        ofSetColor(255);
//        ofRect(0, 0, minRect.size.width, minRect.size.height);
        
        int tmpSizeX = minRect.size.width * ofGetWidth() / grayImage.getWidth();
        int tmpSizeY = minRect.size.height * ofGetHeight() / grayImage.getHeight();
        if( minRect.size.width < minRect.size.height){
            texImage.draw(0, 0, tmpSizeX, tmpSizeY);
        }else{
            texImageR.draw(0, 0, tmpSizeX, tmpSizeY);
        }
        ofPopMatrix();
    }
    ofDisableBlendMode();
    
    preMinRect = minRect;
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    gui.draw();
    
	ofSetColor(255);
  //  srcImage.draw(0, 0, ofGetWidth(), ofGetHeight());
   // bgImage.draw(330, 500, 320, 240);
    //fsImage.draw(660, 500, 320, 240);
}

//-----------------------------------------
void testApp::fitBox( vector <ofPoint>  & contour, CvBox2D32f & box)
{
    
    
    CvPoint * pointArray = new CvPoint[contour.size()];
    
    for( int i = 0; i < contour.size(); i++){
        
        pointArray[i].x =   contour[i].x;
        pointArray[i].y =   contour[i].y;
    }
    
    CvMat pointMat = cvMat( 1, contour.size(), CV_32SC2, pointArray);
    
    box = cvMinAreaRect2(&pointMat, 0);
    
    free(pointArray);
}

//--------------------------------------------------------------
void testApp::exit() {
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
    uint32_t min = 0, max = 0;
    if( key == 'h'){
        gui.hidden = !gui.hidden;
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    gui.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    gui.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
    gui.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
