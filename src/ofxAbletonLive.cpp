#include "ofxAbletonLive.h"


void ofxAbletonLive::setup(string abletonOscHost)
{
    this->abletonOscHost = abletonOscHost;
    loaded = false;
    
    sender.setup(abletonOscHost, ABLETON_OSC_PORT_OUT);
    receiver.setup(ABLETON_OSC_PORT_IN);
    
    tempo.addListener(this, &ofxAbletonLive::eventTempo);
    time.addListener(this, &ofxAbletonLive::eventTime);
    overdub.addListener(this, &ofxAbletonLive::eventOverdub);
    volume.addListener(this, &ofxAbletonLive::eventVolume);
    pan.addListener(this, &ofxAbletonLive::eventPan);
    crossfade.addListener(this, &ofxAbletonLive::eventCrossfade);

    tempo.set("tempo", 120, 4, 999);
    time.set("time", 0, 0, 999);
    overdub.set("overdub", false);
    volume.set("volume", 1, 0, 1);
    pan.set("pan", 0, -1, 1);
    crossfade.set("crossfade", 0, -1, 1);
    
    scanLiveSet();
}

void ofxAbletonLive::clear()
{
    loaded = false;
    
    map<int, ofxAbletonLiveTrack*>::iterator it = tracks.begin();
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = returnTracks.begin();
    while (it != tracks.end())
    {
        delete it->second;
        tracks.erase(it++);
    }
    while (itr != returnTracks.end())
    {
        delete itr->second;
        returnTracks.erase(itr++);
    }
    tracks.clear();
    returnTracks.clear();
    tracksLU.clear();
    returnTracksLU.clear();
}

void ofxAbletonLive::refresh()
{
    clear();
    scanLiveSet();
}

void ofxAbletonLive::scanLiveSet()
{
    requestNumScenes();
    requestNumTracks();
}

void ofxAbletonLive::addNewTrack(int track, string name)
{
    tracks[track] = new ofxAbletonLiveTrack(name, track, &sender);
    tracks[track]->setupTrack();
    tracksLU[name] = tracks[track];
    initializeSends(tracks[track], false);
}

void ofxAbletonLive::addNewReturnTrack(int track)
{
    string name = "Return "+ofToString(track);
    ofxAbletonLiveReturnTrack *newTrack = new ofxAbletonLiveReturnTrack(name, track, &sender);
    newTrack->setupTrack();
    initializeSends(newTrack, true);
    returnTracks[track] = newTrack;
    returnTracksLU[name] = returnTracks[track];
    addSend(track);
}

void ofxAbletonLive::addSend(int track)
{
    map<int, ofxAbletonLiveTrack*>::iterator it = tracks.begin();
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = returnTracks.begin();
    for (; it != tracks.end(); ++it) {
        it->second->addSend(track, false);
    }
    for (; itr != returnTracks.end(); ++itr) {
        itr->second->addSend(track, true);
    }
}

void ofxAbletonLive::initializeSends(ofxAbletonLiveTrack *track, bool isReturn)
{
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = returnTracks.begin();
    for (; itr != returnTracks.end(); ++itr) {
        track->addSend(itr->first, isReturn);
    }
}

void ofxAbletonLive::requestNumScenes()
{
    ofxOscMessage msg;
    msg.setAddress("/live/scenes");
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestNumTracks()
{
    ofxOscMessage msg;
    msg.setAddress("/live/tracks");
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestTrack(int track)
{
    ofxOscMessage msg;
    msg.setAddress("/live/name/track");
    msg.addIntArg(track);
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestClipsList()
{
    ofxOscMessage msg;
    msg.setAddress("/live/name/clip");
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestDeviceList(int track, bool isReturn)
{
    ofxOscMessage msg;
    msg.setAddress(isReturn ? "/live/return/devicelist" : "/live/devicelist");
    msg.addIntArg(track);
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestDeviceParameters(int track, int device, bool isReturn)
{
    ofxOscMessage msg;
    msg.setAddress(isReturn ? "/live/return/device" : "/live/device");
    msg.addIntArg(track);
    msg.addIntArg(device);
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestDeviceParametersRange(int track, int device, bool isReturn)
{
    ofxOscMessage msg;
    msg.setAddress(isReturn ? "/live/return/device/range" : "/live/device/range");
    msg.addIntArg(track);
    msg.addIntArg(device);
    sender.sendMessage(msg);
}

void ofxAbletonLive::selectScene(int scene)
{
    ofxOscMessage msg;
    msg.setAddress("/live/play/scene");
    msg.addIntArg(scene);
    sender.sendMessage(msg);
}

void ofxAbletonLive::update()
{
    while(receiver.hasWaitingMessages())
    {
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        displayOscMessage(m);
        if      (m.getAddress() == "/live/device/param") {
            processParameterUpdate(m);
        }
        else if (m.getAddress() == "/live/name/clip") {
            processClip(m);
        }
        else if (m.getAddress() == "/live/devicelist") {
            processDeviceList(m, false);
        }
        else if (m.getAddress() == "/live/device/allparam") {
            processDeviceParameters(m, false);
        }
        else if (m.getAddress() == "/live/device/range") {
            processDeviceParametersRange(m, false);
        }
        else if (m.getAddress() == "/live/return/devicelist") {
            processDeviceList(m, true);
        }
        else if (m.getAddress() == "/live/return/device/allparam") {
            processDeviceParameters(m, true);
        }
        else if (m.getAddress() == "/live/return/device/range") {
            processDeviceParametersRange(m, true);
        }
        else if (m.getAddress() == "/live/scenes") {
            processNumScenes(m);
        }
        else if (m.getAddress() == "/live/tracks") {
            processNumTracks(m);
        }
        else if (m.getAddress() == "/live/name/track") {
            processTrack(m);
        }
    }
}

void ofxAbletonLive::processClip(ofxOscMessage &m)
{
    int track = m.getArgAsInt32(0);
    int clip = m.getArgAsInt32(1);
    string clipName = m.getArgAsString(2);
    if (tracks.count(track) != 0 && tracks[track]->getClips().count(clip) == 0) {
        tracks[track]->addClip(clipName, clip);
    }
}

void ofxAbletonLive::processDeviceList(ofxOscMessage &m, bool isReturn)
{
    int numArgs = m.getNumArgs();
    int track = m.getArgAsInt32(0);
    
    if (isReturn && returnTracks.count(track) == 0) {
        addNewReturnTrack(track);
    }
    
    for (int i = 1; i < numArgs; i+=2)
    {
        int device = m.getArgAsInt32(i);
        string name = m.getArgAsString(i+1);
        if (!isReturn && tracks[track]->getDevices().count(device) == 0) {
            tracks[track]->addDevice(name, device);
        }
        else if (isReturn && returnTracks[track]->getDevices().count(device) == 0) {
            returnTracks[track]->addDevice(name, device);
        }
        requestDeviceParameters(track, device, isReturn);
        requestDeviceParametersRange(track, device, isReturn);
    }
    
    requestClipsList();
}

void ofxAbletonLive::processDeviceParameters(ofxOscMessage &m, bool isReturn)
{
    int track = m.getArgAsInt32(0);
    int device = m.getArgAsInt32(1);
    if (!isReturn && tracks.count(track) != 0 && tracks[track]->getDevices().count(device) != 0) {
        tracks[track]->getDevices()[device]->receiveParameterValues(m);
    }
    else if (isReturn && returnTracks.count(track) != 0 && returnTracks[track]->getDevices().count(device) != 0) {
        returnTracks[track]->getDevices()[device]->receiveParameterValues(m, true);
    }
}

void ofxAbletonLive::processDeviceParametersRange(ofxOscMessage &m, bool isReturn)
{
    int track = m.getArgAsInt32(0);
    int device = m.getArgAsInt32(1);
    if (!isReturn && tracks.count(track) != 0 && tracks[track]->getDevices().count(device) != 0) {
        tracks[track]->getDevices()[device]->receiveParameterRanges(m);
    }
    else if (isReturn && returnTracks.count(track) != 0 && returnTracks[track]->getDevices().count(device) != 0) {
        returnTracks[track]->getDevices()[device]->receiveParameterRanges(m);
    }
    checkIfTracksLoaded();
}

void ofxAbletonLive::checkIfTracksLoaded()
{
    for (int t=0; t<numTracks; t++)
    {
        map<int, ofxAbletonLiveDevice*>::iterator itd = tracks[t]->getDevices().begin();
        for (; itd != tracks[t]->getDevices().end(); ++itd) {
            if (!itd->second->getInitialized()) return;
        }
    }
    for (int t=0; t<returnTracks.size(); t++)
    {
        map<int, ofxAbletonLiveDevice*>::iterator itd = returnTracks[t]->getDevices().begin();
        for (; itd != returnTracks[t]->getDevices().end(); ++itd) {
            if (!itd->second->getInitialized()) return;
        }
    }
    loaded = true;
    ofNotifyEvent(abletonLoadedE);
}

void ofxAbletonLive::processParameterUpdate(ofxOscMessage &m)
{
    int track = m.getArgAsInt32(0);
    int device = m.getArgAsInt32(1);
    int parameter = m.getArgAsInt32(2);
    if (parameter > 0 && tracks.count(track) != 0 && tracks[track]->getDevices().count(device)) {
        tracks[track]->getDevices()[device]->getParameterGroup()->getFloat(parameter).set(m.getArgAsFloat(3));
    }
}

void ofxAbletonLive::processNumScenes(ofxOscMessage &m)
{
    this->numScenes = m.getArgAsInt32(0);
}

void ofxAbletonLive::processNumTracks(ofxOscMessage &m)
{
    numTracks = m.getArgAsInt32(0);
    for (int t = 0; t < numTracks; t++) {
        requestTrack(t);
    }
    // no way to request number of returns, so try the maximum
    for (int i=0; i<12; i++) {
        requestDeviceList(i, true);
    }
}

void ofxAbletonLive::processTrack(ofxOscMessage &m)
{
    int track = m.getArgAsInt32(0);
    string name = m.getArgAsString(1);
    addNewTrack(track, name);
    requestDeviceList(track);
}

void ofxAbletonLive::prevCue()
{
    ofxOscMessage msg;
    msg.setAddress("/live/prev/cue");
    sender.sendMessage(msg);
}

void ofxAbletonLive::nextCue()
{
    ofxOscMessage msg;
    msg.setAddress("/live/next/cue");
    sender.sendMessage(msg);
}

void ofxAbletonLive::play()
{
    ofxOscMessage msg;
    msg.setAddress("/live/play");
    sender.sendMessage(msg);
}

void ofxAbletonLive::playContinue()
{
    ofxOscMessage msg;
    msg.setAddress("/live/play/continue");
    sender.sendMessage(msg);
}

void ofxAbletonLive::playSelection()
{
    ofxOscMessage msg;
    msg.setAddress("/live/play/selection");
    sender.sendMessage(msg);
}

void ofxAbletonLive::stop()
{
    ofxOscMessage msg;
    msg.setAddress("/live/stop");
    sender.sendMessage(msg);
}

void ofxAbletonLive::setTempo(float tempo)
{
    this->tempo = tempo;
    ofxOscMessage msg;
    msg.setAddress("/live/tempo");
    msg.addFloatArg(tempo);
    sender.sendMessage(msg);
}

void ofxAbletonLive::setTime(float time)
{
    this->time = time;
    ofxOscMessage msg;
    msg.setAddress("/live/time");
    msg.addFloatArg(time);
    sender.sendMessage(msg);
}

void ofxAbletonLive::setOverdub(bool overdub)
{
    this->overdub = overdub;
    ofxOscMessage msg;
    msg.setAddress("/live/overdub");
    msg.addIntArg(overdub ? 1 : 0);
    sender.sendMessage(msg);
}

void ofxAbletonLive::setVolume(float volume)
{
    this->volume = volume;
    ofxOscMessage msg;
    msg.setAddress("/live/master/volume");
    msg.addFloatArg(volume);
    sender.sendMessage(msg);
}

void ofxAbletonLive::setPan(float pan)
{
    this->pan = pan;
    ofxOscMessage msg;
    msg.setAddress("/live/master/pan");
    msg.addFloatArg(pan);
    sender.sendMessage(msg);
}

void ofxAbletonLive::setCrossFader(float crossfade)
{
    ofxOscMessage msg;
    msg.setAddress("/live/master/crossfader");
    msg.addFloatArg(crossfade);
    sender.sendMessage(msg);
}

void ofxAbletonLive::setQuantization(int quantization)
{
    ofxOscMessage msg;
    msg.setAddress("/live/quantization");
    msg.addIntArg(ofClamp(quantization, 0, 13));
    sender.sendMessage(msg);
}

void ofxAbletonLive::displayOscMessage(ofxOscMessage &m)
{
    string display = "Osc : " + m.getAddress() + " : ";
    for (int i = 0; i < m.getNumArgs(); i++)
    {
        if      (m.getArgType(i) == OFXOSC_TYPE_INT32) {
            display += "(i) "+ofToString(m.getArgAsInt32(i))+",";
        }
        else if (m.getArgType(i) == OFXOSC_TYPE_INT64) {
            display += "(i) "+ofToString(m.getArgAsInt64(i))+",";
        }
        else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
            display += "(f) "+ofToString(m.getArgAsFloat(i))+",";
        }
        else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
            display += "(s) "+ofToString(m.getArgAsString(i))+",";
        }
        else if (m.getArgType(i) == OFXOSC_TYPE_BLOB) {
            display += "(blob) "+ofToString(m.getArgAsBlob(i))+",";
        }
    }
    ofLog(OF_LOG_NOTICE, display);
}

ofxAbletonLiveTrack * ofxAbletonLive::getTrack(int track)
{
    if (tracks.count(track) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access track which does not exist!");
        return NULL;
    }
    else {
        return tracks[track];
    }
}

ofxAbletonLiveTrack * ofxAbletonLive::getTrack(string name)
{
    if (tracksLU.count(name) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access track which does not exist!");
        return NULL;
    }
    else {
        return tracksLU[name];
    }
}

ofxAbletonLiveReturnTrack * ofxAbletonLive::getReturnTrack(int track)
{
    if (tracks.count(track) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access return track which does not exist!");
        return NULL;
    }
    else {
        return returnTracks[track];
    }
}

ofxAbletonLiveReturnTrack * ofxAbletonLive::getReturnTrack(string name)
{
    if (tracksLU.count(name) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access return track which does not exist!");
        return NULL;
    }
    else {
        return returnTracksLU[name];
    }
}

string ofxAbletonLive::getTrackString()
{
    map<int, ofxAbletonLiveTrack*>::iterator it = tracks.begin();
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = returnTracks.begin();
    string s;
    s += "Tracks: \n";
    for (; it != tracks.end(); ++it) {
        s += (" -> ("+ ofToString(it->first) +") \""+ it->second->getName() + "\"\n");
    }
    s += "Return Tracks: \n";
    for (; itr != returnTracks.end(); ++itr) {
        s += (" -> (" + ofToString(itr->first) + ") \"" + itr->second->getName() + "\"\n");
    }
    return s;
}

string ofxAbletonLive::getAllString()
{
    string s = getTrackString() + "\n";
    map<int, ofxAbletonLiveTrack*>::iterator it = tracks.begin();
    for (; it != tracks.end(); ++it)
    {
        string trackString;
        trackString += it->second->getDevicesInfo();
        trackString += it->second->getClipsInfo();
        
        map<int, ofxAbletonLiveDevice*>::iterator itd = it->second->getDevices().begin();
        for (; itd != it->second->getDevices().end(); ++itd) {
            trackString += itd->second->getParametersInfo();
        }

        s += trackString;
    }
    return s;
}

ofxAbletonLive::~ofxAbletonLive()
{
    abletonLoadedE.clear();
    abletonLoadedE.disable();
    
    tempo.removeListener(this, &ofxAbletonLive::eventTempo);
    time.removeListener(this, &ofxAbletonLive::eventTime);
    overdub.removeListener(this, &ofxAbletonLive::eventOverdub);
    volume.removeListener(this, &ofxAbletonLive::eventVolume);
    pan.removeListener(this, &ofxAbletonLive::eventPan);
    crossfade.removeListener(this, &ofxAbletonLive::eventCrossfade);

    clear();
}
