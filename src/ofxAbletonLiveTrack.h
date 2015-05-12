#pragma once

#include "ofxOsc.h"

#include "ofxAbletonLiveClip.h"
#include "ofxAbletonLiveDevice.h"

class ofxAbletonLiveTrack
{
    friend class ofxAbletonLive;
public:
    
    struct ofxAbletonLiveSend
    {
        int track;
        int index;
        ofxOscSender *sender;
        string addrSend;
        ofParameter<float> send;

        ofxAbletonLiveSend(int track, int index, bool isReturn, ofxOscSender * sender);
        ~ofxAbletonLiveSend();
        void setSend(float sendValue);
        void eventSend(float &v) {setSend(send);}
    };
    
    ofxAbletonLiveTrack(string name, int track, ofxOscSender * sender);
    ~ofxAbletonLiveTrack();
    
    string getName() {return name;}
    int getTrackIndex() {return track;}

    bool isReturn() {return isReturnTrack;}
    
    void selectClip(int clip);
    void stop();
    
    void setArmed(bool arm);
    void setMute(bool mute);
    void setSolo(bool solo);
    void setVolume(float volume);
    void setPan(float pan);
    void setSend(int index, float send);
        
    ofParameter<bool> & getArmed() {return arm;}
    ofParameter<bool> & getMute() {return mute;}
    ofParameter<bool> & getSolo() {return solo;}
    ofParameter<float> & getVolume() {return volume;}
    ofParameter<float> & getPan() {return pan;}
    
    ofxAbletonLiveClip * getClip(int index);
    ofxAbletonLiveDevice * getDevice(int index);
    ofxAbletonLiveSend * getSend(int index);
    ofxAbletonLiveClip * getClip(string name);
    ofxAbletonLiveDevice * getDevice(string name);
    
    map<int, ofxAbletonLiveClip*> & getClips() {return clips;}
    map<int, ofxAbletonLiveDevice*> & getDevices() {return devices;}
    map<int, ofxAbletonLiveSend*> & getSends() {return sends;}
    
    string getClipsInfo();
    string getDevicesInfo();
    void printClips() {cout << getClipsInfo() << endl;}
    void printDevices() {cout << getDevicesInfo() << endl;}
    
protected:

    void setupTrack();
    void clear();
    void addDevice(string name, int device);
    void addClip(string name, int clip);
    void addSend(int send, bool isReturn);
    
    void eventSetArmed(bool &b) {setArmed(arm);}
    void eventSetMute(bool &b) {setMute(mute);}
    void eventSetSolo(bool &b) {setSolo(solo);}
    void eventSetVolume(float &v) {setVolume(volume);}
    void eventSetPan(float &v) {setPan(pan);}
    
    string name;
    int track;

    map<int, ofxAbletonLiveClip*> clips;
    map<int, ofxAbletonLiveDevice*> devices;
    map<int, ofxAbletonLiveSend*> sends;
    map<string, ofxAbletonLiveClip*> clipsLU;
    map<string, ofxAbletonLiveDevice*> devicesLU;
    
    ofxOscSender * sender;

    ofParameter<bool> arm;
    ofParameter<bool> mute;
    ofParameter<bool> solo;
    ofParameter<float> volume;
    ofParameter<float> pan;
    
    bool isReturnTrack;
    string addrStop, addrClip, addrArm, addrMute, addrSolo, addrVolume, addrPan, addrSend;
};



class ofxAbletonLiveReturnTrack : public ofxAbletonLiveTrack
{
public:
    ofxAbletonLiveReturnTrack(string name, int track, ofxOscSender * sender) : ofxAbletonLiveTrack(name, track, sender)
    {
        addrStop = "/live/return/stop"; // not implemented
        addrClip = "/live/return/play/clipslot"; // not implemented
        addrArm = "/live/return/arm"; // not implemented
        addrMute = "/live/return/mute";
        addrSolo = "/live/return/solo";
        addrVolume = "/live/return/volume";
        addrPan = "/live/return/pan";
        addrSend = "/live/return/send";
        isReturnTrack = true;
    }
};
