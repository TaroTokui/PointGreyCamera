//
//  VectorField.h
//  ofxKinectExample
//
//  Created by tokui on 13/02/11.
//
//

#pragma once
#include "ofMain.h"

class vectorField{
public:
    vectorField();
    ~vectorField();
    
    void setupField(int innerW, int innerH, int outerW, int outerH);
    void clear();
    
    void fadeField(float fadeAmount);
    void randomizeField(float scale);
    
    void draw();
    
    ofVec2f getForceFromPos(float xpos, float ypos);
    
    void addInwardCircle(float x, float y, float radius, float strength);
    void addOutwardCircle(float x, float y, float radius, float strength);
    void addClockwiseCircle(float x, float y, float radius, float strength);
    void addCounterClockwiseCircle(float x, float y, float radius, float strength);
    void addVectorCircle(float x, float y, float vx, float vy, float radius, float strength);
    
    void setFromPixels(unsigned char * pixels, bool bMoveTowardsWhite, float strength);
    
private:
    int fieldWidth;
    int fieldHeight;
    int externalWidth;
    int externalHeight;
    int fieldSize;
    
    vector<ofVec2f>field;
};