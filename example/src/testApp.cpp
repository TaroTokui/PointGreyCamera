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
    
    gui.addPanel("Resize", 1);
    gui.setWhichPanel(1);
    gui.addSlider("X1", "x1", 0, 0, 200, true);
    gui.addSlider("Y1", "y1", 0, 0, 200, true);
    gui.addSlider("X2", "x2", ofGetWidth(), ofGetWidth()/2, ofGetWidth(), true);
    gui.addSlider("Y2", "y2", ofGetHeight(), ofGetHeight()/2, ofGetHeight(), true);
    
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
    
    ard.connect("/dev/tty.usbmodemfd121", 57600);
	ofAddListener(ard.EInitialized, this, &testApp::setupArduino);
	bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
	
	fbo.begin();
	ofClear(255,255,255, 0);
    fbo.end();
  // 2013/03/03
//    fboPixels = (unsigned char*)malloc(ofGetWidth() * ofGetHeight());
}

//--------------------------------------------------------------
void testApp::update() {
	
    pgcamera->update();
    grayImage = pgcamera->getGrayImage();
    srcImage = grayImage;

    fsPixels = grayImage.getPixels();
    
    int width = grayImage.width;
    int height = grayImage.height;
    
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
    updateArduino();
    
    fbo.begin();
    drawFbo();
    fbo.end();
    
    // 2013/03/03
//    ofPixels fboPixels;
//    fbo.readToPixels(fboPixels);
//    
//    int w = fbo.getWidth();
//    int h = fbo.getHeight();
//    for (int j=0; j<h; j++) {
//        for (int i=0; i<w; i++) {
//            if( fboPixels[j*w*3 + i] == 255 ) fboPixels[j*w*3 + i] = 0;
//        }
//    }
//    fboImage.setFromPixels(fboPixels, w, h);
    
}

//--------------------------------------------------------------
void testApp::draw() {
    ofSetRectMode(OF_RECTMODE_CORNER);
    fbo.draw(gui.getValueI("x1"), gui.getValueI("y1"), gui.getValueI("x2"), gui.getValueI("y2"));
    gui.draw();
    
	ofSetColor(0xffffff);
    //  srcImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    // bgImage.draw(330, 500, 320, 240);
    //fsImage.draw(660, 500, 320, 240);
}

//--------------------------------------------------------------
void testApp::drawFbo() {
	
    ofSetRectMode(OF_RECTMODE_CENTER);
	ofSetColor(0xffffff);
	
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
        ofSetColor(0xffffff);
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


//--------------------------------------------------------------
void testApp::setupArduino(const int & version) {
	
	// remove listener because we don't need it anymore
	ofRemoveListener(ard.EInitialized, this, &testApp::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    cout << ard.getFirmwareName() << endl;
    cout << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion() << endl;
    
    // Note: pins A0 - A5 can be used as digital input and output.
    // Refer to them as pins 14 - 19 if using StandardFirmata from Arduino 1.0.
    // If using Arduino 0022 or older, then use 16 - 21.
    // Firmata pin numbering changed in version 2.3 (which is included in Arduino 1.0)
    
    // set pins D2 and A5 to digital input
    ard.sendDigitalPinMode(2, ARD_INPUT);
    ard.sendDigitalPinMode(19, ARD_INPUT);  // pin 21 if using StandardFirmata from Arduino 0022 or older
    
    // set pin A0 to analog input
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    
    // set pin D13 as digital output
	ard.sendDigitalPinMode(13, ARD_OUTPUT);
    // set pin A4 as digital output
    ard.sendDigitalPinMode(18, ARD_OUTPUT);  // pin 20 if using StandardFirmata from Arduino 0022 or older
    
    // set pin D11 as PWM (analog output)
	ard.sendDigitalPinMode(11, ARD_PWM);
    
    // attach a servo to pin D9
    // servo motors can only be attached to pin D3, D5, D6, D9, D10, or D11
    ard.sendServoAttach(9);
	
    // Listen for changes on the digital and analog pins
    ofAddListener(ard.EDigitalPinChanged, this, &testApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &testApp::analogPinChanged);
}

//--------------------------------------------------------------
void testApp::updateArduino(){
    
	// update the arduino, get any data or messages.
    // the call to ard.update() is required
	ard.update();
	
	// do not send anything until the arduino has been set up
	if (bSetupArduino) {
        // fade the led connected to pin D11
		ard.sendPwm(11, (int)(128 + 128 * sin(ofGetElapsedTimef())));   // pwm...
	}
    
}

// digital pin event handler, called whenever a digital pin value has changed
// note: if an analog pin has been set as a digital pin, it will be handled
// by the digitalPinChanged function rather than the analogPinChanged function.

//--------------------------------------------------------------
void testApp::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
}

// analog pin event handler, called whenever an analog pin value has changed

//--------------------------------------------------------------
void testApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = "analog pin: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
}
