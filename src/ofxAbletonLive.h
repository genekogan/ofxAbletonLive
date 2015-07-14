#pragma once

#include "ofxOsc.h"

#include "ofxAbletonLiveParameter.h"
#include "ofxAbletonLiveDevice.h"
#include "ofxAbletonLiveClip.h"
#include "ofxAbletonLiveTrack.h"

#define ABLETON_OSC_HOST_DEFAULT "localhost"
#define ABLETON_OSC_PORT_OUT 9000
#define ABLETON_OSC_PORT_IN 9001


class ofxAbletonLive
{
public:
    ~ofxAbletonLive();
    
    void setup(string abletonOscHost = ABLETON_OSC_HOST_DEFAULT);
    template<typename L, typename M>
    void setup(string abletonOscHost, L *listener, M method);
    template<typename L, typename M>
    void setup(L *listener, M method);

    template<typename L, typename M>
    void refresh(L *listener, M method);
    void refresh();
    bool isLoaded() {return loaded;}
    
    void update();
    
    void drawDebugView();

    void play();
    void playContinue();
    void playSelection();
    void stop();
    void selectScene(int scene);
    void prevCue();
    void nextCue();
    
    void setTempo(float tempo);
    void setTime(float time);
    void setOverdub(bool overdub);
    void setVolume(float volume);
    void setPan(float pan);
    void setCrossFader(float crossfade);
    void setQuantization(int quantization);
    
    ofParameter<float> & getTempo() {return tempo;}
    ofParameter<float> & getTime() {return time;}
    ofParameter<bool> & getOverdub() {return overdub;}
    ofParameter<float> & getVolume() {return volume;}
    ofParameter<float> & getPan() {return pan;}
    ofParameter<float> & getCrossFader() {return crossfade;}

    int getNumTracks() {return numTracks;}
    int getNumScenes() {return numScenes;}

    map<int, ofxAbletonLiveTrack*> & getTracks() {return tracks;}
    map<int, ofxAbletonLiveReturnTrack*> & getReturnTracks() {return returnTracks;}

    ofxAbletonLiveTrack * getTrack(int track);
    ofxAbletonLiveTrack * getTrack(string name);
    ofxAbletonLiveReturnTrack * getReturnTrack(int track);
    ofxAbletonLiveReturnTrack * getReturnTrack(string name);
    
    string getTrackString();
    string getAllString();
    void printTracks() {cout<<getTrackString()<<endl;}
    void printAll() {cout<<getAllString()<<endl;}
    
protected:

    void clear();
    void scanLiveSet();

    void addNewTrack(int track, string name);
    void addNewReturnTrack(int track);
    void addSend(int track);
    void initializeSends(ofxAbletonLiveTrack *track, bool isReturn);
    void checkIfTracksLoaded();
    void displayOscMessage(ofxOscMessage &m);

    void requestNumScenes();
    void requestNumTracks();
    void requestTrack(int track);
    void requestClipsList();
    void requestDeviceList(int track, bool isReturn=false);
    void requestDeviceParameters(int track, int device, bool isReturn=false);
    void requestDeviceParametersRange(int track, int device, bool isReturn=false);

    void processClip(ofxOscMessage &m);
    void processDeviceList(ofxOscMessage &m, bool isReturn);
    void processDeviceParameters(ofxOscMessage &m, bool isReturn);
    void processDeviceParametersRange(ofxOscMessage &m, bool isReturn);

    void processParameterUpdate(ofxOscMessage &m);
    void processNumScenes(ofxOscMessage &m);
    void processNumTracks(ofxOscMessage &m);
    void processTrack(ofxOscMessage &m);

    void eventTempo(float &v) {setTempo(tempo);}
    void eventTime(float &v) {setTime(time);}
    void eventOverdub(bool &v) {setOverdub(overdub);}
    void eventVolume(float &v) {setVolume(volume);}
    void eventPan(float &v) {setPan(pan);}
    void eventCrossfade(float &v) {setCrossFader(crossfade);}

    map<int, ofxAbletonLiveTrack*> tracks;
    map<int, ofxAbletonLiveReturnTrack*> returnTracks;
    map<string, ofxAbletonLiveTrack*> tracksLU;
    map<string, ofxAbletonLiveReturnTrack*> returnTracksLU;
    int numTracks;
    int numScenes;

    ofParameter<float> tempo;
    ofParameter<float> time;
    ofParameter<bool> overdub;
    ofParameter<float> volume;
    ofParameter<float> pan;
    ofParameter<float> crossfade;
    
    ofxOscSender sender;
    ofxOscReceiver receiver;
    string abletonOscHost;
    bool loaded;
    ofEvent<void> abletonLoadedE;
};


template<typename L, typename M>
void ofxAbletonLive::setup(string abletonOscHost, L *listener, M method)
{
    ofAddListener(abletonLoadedE, listener, method);
    setup(abletonOscHost);
}

template<typename L, typename M>
void ofxAbletonLive::setup(L *listener, M method)
{
    ofAddListener(abletonLoadedE, listener, method);
    setup();
}

template<typename L, typename M>
void ofxAbletonLive::refresh(L *listener, M method)
{
    abletonLoadedE.clear();
    ofAddListener(abletonLoadedE, listener, method);
    clear();
    scanLiveSet();
}
