#include "ofxAbletonLiveTrack.h"


ofxAbletonLiveTrack::ofxAbletonLiveSend::ofxAbletonLiveSend(int track, int index, bool isReturn, ofxOscSender * sender)
{
    this->track = track;
    this->index = index;
    this->sender = sender;
    send.set("Send "+ofToString(index), 0, 0, 1);
    send.addListener(this, &ofxAbletonLiveSend::eventSend);
    addrSend = isReturn ? "/live/return/send" : "/live/send";
}

ofxAbletonLiveTrack::ofxAbletonLiveSend::~ofxAbletonLiveSend()
{
    send.removeListener(this, &ofxAbletonLiveSend::eventSend);
}

void ofxAbletonLiveTrack::ofxAbletonLiveSend::setSend(float sendValue)
{
    this->send = sendValue;
    ofxOscMessage msg;
    msg.setAddress(addrSend);
    msg.addIntArg(track);
    msg.addIntArg(index);
    msg.addFloatArg(sendValue);
    sender->sendMessage(msg);
}

ofxAbletonLiveTrack::ofxAbletonLiveTrack(string name, int track, ofxOscSender * sender)
{
    this->name = name;
    this->track = track;
    this->sender = sender;
    
    addrStop = "/live/stop";
    addrClip = "/live/play/clipslot";
    addrArm = "/live/arm";
    addrMute = "/live/mute";
    addrSolo = "/live/solo";
    addrVolume = "/live/volume";
    addrPan = "/live/pan";
    addrSend = "/live/send";
    
    isReturnTrack = false;
}

void ofxAbletonLiveTrack::setupTrack()
{
    arm.set("armed", false);
    mute.set("mute", false);
    solo.set("solo", false);
    volume.set("volume", 0.5, 0.0, 1.0);
    pan.set("pan", 0.0, -1.0, 1.0);

    arm.addListener(this, &ofxAbletonLiveTrack::eventSetArmed);
    mute.addListener(this, &ofxAbletonLiveTrack::eventSetMute);
    solo.addListener(this, &ofxAbletonLiveTrack::eventSetSolo);
    volume.addListener(this, &ofxAbletonLiveTrack::eventSetVolume);
    pan.addListener(this, &ofxAbletonLiveTrack::eventSetPan);
}

void ofxAbletonLiveTrack::addDevice(string name, int device)
{
    if (devices.count(device) == 0) {
        devices[device] = new ofxAbletonLiveDevice(name, track, device, sender);
        devicesLU[name] = devices[device];
    }
}

void ofxAbletonLiveTrack::addClip(string name, int clip)
{
    if (!isReturnTrack && clips.count(clip) == 0)
    {
        clips[clip] = new ofxAbletonLiveClip(name, track, clip, sender);
        clipsLU[name] = clips[clip];
    }
}

void ofxAbletonLiveTrack::addSend(int send, bool isReturn)
{
    if (sends.count(send) == 0) {
        sends[send] = new ofxAbletonLiveSend(track, send, isReturn, sender);
    }
}

void ofxAbletonLiveTrack::stop()
{
    ofxOscMessage msg;
    msg.setAddress(addrStop);
    msg.addIntArg(track);
    sender->sendMessage(msg);
}

void ofxAbletonLiveTrack::selectClip(int clip)
{
    ofxOscMessage msg;
    msg.setAddress(addrClip);
    msg.addIntArg(track);
    msg.addIntArg(clip);
    sender->sendMessage(msg);
}

void ofxAbletonLiveTrack::setArmed(bool arm)
{
    this->arm = arm;
    ofxOscMessage msg;
    msg.setAddress(addrArm);
    msg.addIntArg(track);
    msg.addIntArg(arm ? 1 : 0);
    sender->sendMessage(msg);
}

void ofxAbletonLiveTrack::setMute(bool mute)
{
    this->mute = mute;
    ofxOscMessage msg;
    msg.setAddress(addrMute);
    msg.addIntArg(track);
    msg.addIntArg(mute ? 1 : 0);
    sender->sendMessage(msg);
}

void ofxAbletonLiveTrack::setSolo(bool solo)
{
    this->solo = solo;
    ofxOscMessage msg;
    msg.setAddress(addrSolo);
    msg.addIntArg(track);
    msg.addIntArg(solo ? 1 : 0);
    sender->sendMessage(msg);
}

void ofxAbletonLiveTrack::setVolume(float volume)
{
    this->volume = volume;
    ofxOscMessage msg;
    msg.setAddress(addrVolume);
    msg.addIntArg(track);
    msg.addFloatArg(volume);
    sender->sendMessage(msg);
}

void ofxAbletonLiveTrack::setPan(float pan)
{
    this->pan = pan;
    ofxOscMessage msg;
    msg.setAddress(addrPan);
    msg.addIntArg(track);
    msg.addFloatArg(pan);
    sender->sendMessage(msg);
}

void ofxAbletonLiveTrack::setSend(int index, float send)
{
    if (sends.count(index) != 0) {
        sends[index]->setSend(send);
    }
}

ofxAbletonLiveClip * ofxAbletonLiveTrack::getClip(int index)
{
    if (clips.count(index) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access clip which does not exist!");
        return NULL;
    }
    else {
        return clips[index];
    }
}

ofxAbletonLiveDevice * ofxAbletonLiveTrack::getDevice(int index)
{
    if (devices.count(index) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access device which does not exist!");
        return NULL;
    }
    else {
        return devices[index];
    }

}

ofxAbletonLiveTrack::ofxAbletonLiveSend * ofxAbletonLiveTrack::getSend(int index)
{
    if (sends.count(index) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access send which does not exist!");
        return NULL;
    }
    else {
        return sends[index];
    }
}

ofxAbletonLiveClip * ofxAbletonLiveTrack::getClip(string name)
{
    if (clipsLU.count(name) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access clip which does not exist!");
        return NULL;
    }
    else {
        return clipsLU[name];
    }
}

ofxAbletonLiveDevice * ofxAbletonLiveTrack::getDevice(string name)
{
    if (devicesLU.count(name) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access device which does not exist!");
        return NULL;
    }
    else {
        return devicesLU[name];
    }
}

string ofxAbletonLiveTrack::getClipsInfo()
{
    string s;
    map<int, ofxAbletonLiveClip*>::iterator it = clips.begin();
    s += "Track " + ofToString(track) + " Clips: \n";
    for (; it != clips.end(); ++it) {
        s += (" -> (" + ofToString(it->first) + ") \"" + it->second->getName() + "\"\n");
    }
    return s;
}

string ofxAbletonLiveTrack::getDevicesInfo()
{
    string s;
    map<int, ofxAbletonLiveDevice*>::iterator it = devices.begin();
    s += "Track " + ofToString(track) + " Devices: \n";
    for (; it != devices.end(); ++it) {
        s += (" -> ("+ ofToString(it->first) +") \"" + it->second->getName() + "\"\n");
    }
    return s;
}

void ofxAbletonLiveTrack::clear()
{
    map<int, ofxAbletonLiveClip*>::iterator itc = clips.begin();
    map<int, ofxAbletonLiveDevice*>::iterator itd = devices.begin();
    map<int, ofxAbletonLiveSend*>::iterator its = sends.begin();
    for (; itc != clips.end(); ++itc)
    {
        delete itc->second;
        clips.erase(itc);
    }
    for (; itd != devices.end(); ++itd)
    {
        delete itd->second;
        devices.erase(itd);
    }
    for (; its != sends.end(); ++its)
    {
        delete its->second;
        sends.erase(its);
    }
    clipsLU.clear();
    devicesLU.clear();
}

ofxAbletonLiveTrack::~ofxAbletonLiveTrack()
{
    arm.removeListener(this, &ofxAbletonLiveTrack::eventSetArmed);
    mute.removeListener(this, &ofxAbletonLiveTrack::eventSetMute);
    solo.removeListener(this, &ofxAbletonLiveTrack::eventSetSolo);
    volume.removeListener(this, &ofxAbletonLiveTrack::eventSetVolume);
    pan.removeListener(this, &ofxAbletonLiveTrack::eventSetPan);
    clear();
}