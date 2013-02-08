#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofBackground(0);
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofSetFrameRate(60);
    
    pgcamera = new PGRCamera();
    pgcamera->setup();
    
    gui.setup("Control Panel", 0, 300, ofGetWidth()/2, ofGetHeight()/2);
    gui.addPanel("Setting", 2);
    gui.setWhichColumn(0);
    
    gui.addSlider("Threshold", "threshold", 200, 0, 255, true);
    gui.addSlider("MinArea", "minArea", 1000, 0, 10000, true);
    gui.addSlider("MaxArea", "maxArea", 10000, 0, 10000, true);
    gui.addSlider("MaxBlob", "maxBlob", 10, 0, 50, true);
    
    gui.setWhichColumn(1);
    gui.addDrawableRect("ContourImage", &grayImage, 320, 240);
    
    gui.loadSettings("controlPanelSettings.xml");
}

//--------------------------------------------------------------
void testApp::update() {
	
    pgcamera->update();
    grayImage = pgcamera->getGrayImage();
    grayImage.threshold(gui.getValueI("threshold"));
    contourFinder.findContours(grayImage, gui.getValueI("minArea"), gui.getValueI("maxArea"), gui.getValueI("maxBlob"), false);
	gui.update();
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofSetColor(255, 255, 255);
	
  //  pgcamera->draw();
    grayImage.draw(0, 0, 320, 240);
    contourFinder.draw(0, 0, 320, 240);
    gui.draw();
}

//--------------------------------------------------------------
void testApp::exit() {
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
    uint32_t min = 0, max = 0;}

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
