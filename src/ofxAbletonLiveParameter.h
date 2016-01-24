#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class ofxAbletonLiveParameter
{
    friend class ofxAbletonLiveDevice;
public:
    ofxAbletonLiveParameter(ofParameter<float> *parameter, int track, int device, int index, ofxOscSender *sender, bool skipTrack=false);
    ~ofxAbletonLiveParameter();
    
    string getName() {return parameter->getName();}
    float getMin() {return parameter->getMin();}
    float getMax() {return parameter->getMax();}
    ofParameter<float> * getParameter() {return parameter;}
    
    void setValue(float value);
    
    int getTrackIndex() {return track;}
    int getDeviceIndex() {return device;}
    int getParameterIndex() {return index;}
    string getOscAddress() {return oscAddress;}
    
protected:

    void setOscAddress(string oscAddress) {this->oscAddress = oscAddress;}
    void parameterChanged(float & value);
    
    ofParameter<float> *parameter;
    int track;
    int device;
    int index;
    string oscAddress;
    ofxOscSender *sender;
    bool skipTrack;
};
