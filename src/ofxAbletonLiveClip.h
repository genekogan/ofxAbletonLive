#pragma once

#include "ofxOsc.h"
#include "ofxAbletonLiveParameter.h"

class ofxAbletonLiveClip
{
public:
    ofxAbletonLiveClip(string name, int track, int clip, ofxOscSender *sender);
    
    string getName() {return name;}
    int getTrackIndex() {return track;}
    int getClipIndex() {return clip;}
    
    void play();
    void stop();
  
    void setLoopStart(float loopStart);
    void setLoopEnd(float loopEnd);
    void setLooping(bool looping);
    void setTranspose(int transpose);
    void setDetune(int detune);
    void setWarp(bool warp);

    ofParameter<float> & getLoopStart() {return loopStart;}
    ofParameter<float> & getLoopEnd() {return loopEnd;}
    ofParameter<bool> & getLooping() {return looping;}
    ofParameter<int> & getTranspose() {return transpose;}
    ofParameter<int> & getDetune() {return detune;}
    ofParameter<bool> & getWarp() {return warp;}

    ofParameterGroup & getParameterGroup() {return parameterGroup;}

protected:

    void eventLoopStart(float &v) {setLoopStart(loopStart);}
    void eventLoopEnd(float &v) {setLoopEnd(loopEnd);}
    void eventLooping(bool &v) {setLooping(looping);}
    void eventTranspose(int &v) {setTranspose(transpose);}
    void eventDetune(int &v) {setDetune(detune);}
    void eventWarp(bool &v) {setWarp(warp);}
    void sendTransposeDetune();
    
    ofParameter<float> loopStart;
    ofParameter<float> loopEnd;
    ofParameter<bool> looping;
    ofParameter<int> transpose;
    ofParameter<int> detune;
    ofParameter<bool> warp;

    ofParameterGroup parameterGroup;
    
    string name;
    int track;
    int clip;
    ofxOscSender *sender;
};