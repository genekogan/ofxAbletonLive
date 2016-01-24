#include "ofxAbletonLive.h"


void ofxAbletonLive::setup(string abletonOscHost)
{
    this->abletonOscHost = abletonOscHost;
    loaded = false;
    
    sender.setup(abletonOscHost, ABLETON_OSC_PORT_OUT);
    receiver.setup(ABLETON_OSC_PORT_IN);
    
    tempo.set("tempo", 120, 4, 999);
    time.set("time", 0, 0, 999);
    overdub.set("overdub", false);
    volume.set("volume", 1, 0, 1);
    pan.set("pan", 0, -1, 1);
    crossfade.set("crossfade", 0, -1, 1);
    
    tempo.addListener(this, &ofxAbletonLive::eventTempo);
    time.addListener(this, &ofxAbletonLive::eventTime);
    overdub.addListener(this, &ofxAbletonLive::eventOverdub);
    volume.addListener(this, &ofxAbletonLive::eventVolume);
    pan.addListener(this, &ofxAbletonLive::eventPan);
    crossfade.addListener(this, &ofxAbletonLive::eventCrossfade);
    
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
    initializeSends(newTrack, 1);
    returnTracks[track] = newTrack;
    returnTracksLU[name] = returnTracks[track];
    addSend(track);
}

void ofxAbletonLive::addNewMasterTrack()
{
    masterTrack = new ofxAbletonLiveMasterTrack("Master", &sender);
    masterTrack->setupTrack();
}

void ofxAbletonLive::addSend(int track)
{
    map<int, ofxAbletonLiveTrack*>::iterator it = tracks.begin();
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = returnTracks.begin();
    for (; it != tracks.end(); ++it) {
        it->second->addSend(track, 0);
    }
    for (; itr != returnTracks.end(); ++itr) {
        itr->second->addSend(track, 1);
    }
}

void ofxAbletonLive::initializeSends(ofxAbletonLiveTrack *track, int trackType)
{
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = returnTracks.begin();
    for (; itr != returnTracks.end(); ++itr) {
        track->addSend(itr->first, trackType);
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

void ofxAbletonLive::requestDeviceList(int track, int trackType)
{
    ofxOscMessage msg;
    if      (trackType == 0) msg.setAddress("/live/devicelist");
    else if (trackType == 1) msg.setAddress("/live/return/devicelist");
    else if (trackType == 2) msg.setAddress("/live/master/devicelist");
    if (trackType < 2)  msg.addIntArg(track);
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestDeviceListMaster()
{
    ofxOscMessage msg;
    msg.setAddress("/live/master/devicelist");
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestDeviceParameters(int track, int device, int trackType)
{
    ofxOscMessage msg;
    if      (trackType == 0) msg.setAddress("/live/device");
    else if (trackType == 1) msg.setAddress("/live/return/device");
    else if (trackType == 2) msg.setAddress("/live/master/device");
    if (trackType < 2)  msg.addIntArg(track);
    msg.addIntArg(device);
    sender.sendMessage(msg);
}

void ofxAbletonLive::requestDeviceParametersRange(int track, int device, int trackType)
{
    ofxOscMessage msg;
    if      (trackType == 0) msg.setAddress("/live/device/range");
    else if (trackType == 1) msg.setAddress("/live/return/device/range");
    else if (trackType == 2) msg.setAddress("/live/master/device/range");
    if (trackType < 2)  msg.addIntArg(track);
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
        receiver.getNextMessage(m);
        //displayOscMessage(m);
        if      (m.getAddress() == "/live/device/param") {
            processParameterUpdate(m);
        }
        else if (m.getAddress() == "/live/name/clip") {
            processClip(m);
        }
        else if (m.getAddress() == "/live/devicelist") {
            processDeviceList(m, 0);
        }
        else if (m.getAddress() == "/live/device/allparam") {
            processDeviceParameters(m, 0);
        }
        else if (m.getAddress() == "/live/device/range") {
            processDeviceParametersRange(m, 0);
        }
        else if (m.getAddress() == "/live/return/devicelist") {
            processDeviceList(m, 1);
        }
        else if (m.getAddress() == "/live/return/device/allparam") {
            processDeviceParameters(m, 1);
        }
        else if (m.getAddress() == "/live/return/device/range") {
            processDeviceParametersRange(m, 1);
        }
        else if (m.getAddress() == "/live/master/devicelist") {
            processDeviceList(m, 2);
        }
        else if (m.getAddress() == "/live/master/device") {
            processDeviceParameters(m, 2);
        }
        else if (m.getAddress() == "/live/master/device/range") {
            processDeviceParametersRange(m, 2);
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

void ofxAbletonLive::drawDebugView()
{
    ofPushStyle();
    ofSetColor(0);
    
    if (!isLoaded()) {
        ofDrawBitmapString("ofxAbletonLive has not loaded yet!", 100, 100);
        return;
    }
    
    ofDrawBitmapString(getTrackString(), 10, 15);
    int x = 200;
    map<int, ofxAbletonLiveTrack*>::iterator it = getTracks().begin();
    for (; it != getTracks().end(); ++it)
    {
        string trackString = "Track " + ofToString(it->first) + ": \"" + it->second->getName() + "\"\n";
        trackString += it->second->getDevicesInfo();
        trackString += it->second->getClipsInfo();
        map<int, ofxAbletonLiveDevice*>::iterator itd = it->second->getDevices().begin();
        for (; itd != it->second->getDevices().end(); ++itd) {
            trackString += itd->second->getParametersInfo();
        }
        ofDrawBitmapString(trackString, x, 15);
        x += 300;
    }
    ofPopStyle();
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

void ofxAbletonLive::processDeviceList(ofxOscMessage &m, int trackType)
{
    int numArgs = m.getNumArgs();
    int track = trackType == 2 ? 0 : m.getArgAsInt32(0);
    if (trackType==1 && returnTracks.count(track) == 0) {
        addNewReturnTrack(track);
    }
    
    for (int i = trackType==2 ? 0 : 1; i < numArgs; i+=2)
    {
        int device = m.getArgAsInt32(i);
        string name = m.getArgAsString(i+1);
        if (trackType==0 && tracks[track]->getDevices().count(device) == 0) {
            tracks[track]->addDevice(name, device);
        }
        else if (trackType==1 && returnTracks[track]->getDevices().count(device) == 0) {
            returnTracks[track]->addDevice(name, device);
        }
        else if (trackType==2 && masterTrack->getDevices().count(device) == 0) {
            masterTrack->addDevice(name, device);
        }
        requestDeviceParameters(track, device, trackType);
        requestDeviceParametersRange(track, device, trackType);
    }
    
    requestClipsList();
}

void ofxAbletonLive::processDeviceParameters(ofxOscMessage &m, int trackType)
{
    int track = trackType == 2 ? 0 : m.getArgAsInt32(0);
    int device = m.getArgAsInt32(trackType == 2 ? 0 : 1);
    
    if (trackType==0 && tracks.count(track) != 0 && tracks[track]->getDevices().count(device) != 0) {
        tracks[track]->getDevices()[device]->receiveParameterValues(m, trackType);
    }
    else if (trackType==1 && returnTracks.count(track) != 0 && returnTracks[track]->getDevices().count(device) != 0) {
        returnTracks[track]->getDevices()[device]->receiveParameterValues(m, 1);
    }
    else if (trackType==2 && masterTrack->getDevices().count(device) != 0) {
        masterTrack->getDevices()[device]->receiveParameterValues(m, 2);
    }
}

void ofxAbletonLive::processDeviceParametersRange(ofxOscMessage &m, int trackType)
{
    int track = trackType == 2 ? 0 : m.getArgAsInt32(0);
    int device = m.getArgAsInt32(trackType == 2 ? 0 : 1);
    if (trackType==0 && tracks.count(track) != 0 && tracks[track]->getDevices().count(device) != 0) {
        tracks[track]->getDevices()[device]->receiveParameterRanges(m, trackType);
    }
    else if (trackType==1 && returnTracks.count(track) != 0 && returnTracks[track]->getDevices().count(device) != 0) {
        returnTracks[track]->getDevices()[device]->receiveParameterRanges(m, trackType);
    }
    else if (trackType==2 && masterTrack->getDevices().count(device) != 0) {
        masterTrack->getDevices()[device]->receiveParameterRanges(m, trackType);
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
    map<int, ofxAbletonLiveDevice*>::iterator itd = masterTrack->getDevices().begin();
    for (; itd != masterTrack->getDevices().end(); ++itd) {
        if (!itd->second->getInitialized()) return;
    }

    if (!loaded) {
        loaded = true;
        ofNotifyEvent(abletonLoadedE);
    }
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
    
    // no way to request number of returns, so try the maximum 12
    for (int i=0; i<12; i++) {
        requestDeviceList(i, 1);
    }
    
    // request master track devices
    addNewMasterTrack();
    requestDeviceListMaster();
}

void ofxAbletonLive::processTrack(ofxOscMessage &m)
{
    int track = m.getArgAsInt32(0);
    string name = m.getArgAsString(1);
    addNewTrack(track, name);
    requestDeviceList(track, 0);
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
    abletonLoadedE.disable();
    
    tempo.removeListener(this, &ofxAbletonLive::eventTempo);
    time.removeListener(this, &ofxAbletonLive::eventTime);
    overdub.removeListener(this, &ofxAbletonLive::eventOverdub);
    volume.removeListener(this, &ofxAbletonLive::eventVolume);
    pan.removeListener(this, &ofxAbletonLive::eventPan);
    crossfade.removeListener(this, &ofxAbletonLive::eventCrossfade);
    
    clear();
}
