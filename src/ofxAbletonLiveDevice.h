#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxAbletonLiveParameter.h"


class ofxAbletonLiveDevice
{
    friend class ofxAbletonLive;
public:
    ofxAbletonLiveDevice(string name, int track, int device, ofxOscSender *sender);
    ~ofxAbletonLiveDevice();
    
    string getName() {return name;}
    int getTrackIndex() {return track;}
    int getDeviceIndex() {return device;}

    void setEnabled(bool enabled);
    ofParameter<bool> & getEnabled() {return enabled;}
    
    ofxAbletonLiveParameter * getParameter(int index);
    ofxAbletonLiveParameter * getParameter(string name);
    map<int, ofxAbletonLiveParameter*> & getParameters() {return parameters;}
    ofParameterGroup * getParameterGroup() {return parameterGroup;}
    
    string getParametersInfo();
    void printParameters() {cout << getParametersInfo() << endl;}
    
protected:

    void addParameter(int index, string name, float value, bool isReturn=false);
    void receiveParameterValues(ofxOscMessage &m, bool isReturn=false);
    void receiveParameterRanges(ofxOscMessage &m);
    void clear();
    void eventSetEnabled(bool &b) {setEnabled(enabled);}
    bool getInitialized() {return initialized;}
    
    map<int, ofxAbletonLiveParameter*> parameters;
    map<string, ofxAbletonLiveParameter*> parametersLU;
    ofParameterGroup *parameterGroup;
    
    string name;
    int track;
    int device;
    ofParameter<bool> enabled;
    ofxOscSender *sender;

    bool initialized;
};

