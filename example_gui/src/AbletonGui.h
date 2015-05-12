#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAbletonLive.h"


class AbletonGui
{
public:    
    class AbletonGuiTrack
    {
    public:
        AbletonGuiTrack(ofxAbletonLiveTrack *track);
        void setup();
        
        ofxAbletonLiveTrack *track;
        
        ofxPanel panel;
        ofxGuiGroup panelPlayback;
        ofxGuiGroup panelClips;
        ofxGuiGroup panelDevices;
        ofxGuiGroup panelSends;
        ofxToggle tVisible;
    };
    
    void setup(ofxAbletonLive * live);
    void draw();
    
    vector<AbletonGuiTrack*> & getTracks() {return tracks;}
    
private:
    
    void eventSelectTrack(bool &b);
    void eventTempo(float &v) {live->setTempo(live->getTempo());}
    void eventTime(float &v) {live->setTime(live->getTime());}
    void eventOverdub(bool &v) {live->setOverdub(live->getOverdub());}
    void eventVolume(float &v) {live->setVolume(live->getVolume());}
    void eventPan(float &v) {live->setPan(live->getPan());}
    void eventCrossfade(float &v) {live->setCrossFader(live->getCrossFader());}
    
    vector<AbletonGuiTrack*> tracks;

    ofxPanel gui;
    ofxGuiGroup guiPlayback;
    ofxGuiGroup guiGlobalParameters;
    ofxGuiGroup guiSelectActiveTrack;
    ofxButton bPlay, bContinue, bSelection, bStop, bPrev, bNext;
    
    ofxAbletonLive *live;
};