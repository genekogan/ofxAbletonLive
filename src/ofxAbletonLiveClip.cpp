#include "ofxAbletonLiveClip.h"


ofxAbletonLiveClip::ofxAbletonLiveClip(string name, int track, int clip, ofxOscSender *sender)
{
    this->name = name;
    this->track = track;
    this->clip = clip;
    this->sender = sender;
    
    loopStart.addListener(this, &ofxAbletonLiveClip::eventLoopStart);
    loopEnd.addListener(this, &ofxAbletonLiveClip::eventLoopEnd);
    looping.addListener(this, &ofxAbletonLiveClip::eventLooping);
    transpose.addListener(this, &ofxAbletonLiveClip::eventTranspose);
    detune.addListener(this, &ofxAbletonLiveClip::eventDetune);
    warp.addListener(this, &ofxAbletonLiveClip::eventWarp);

    parameterGroup.setName(name);
    parameterGroup.add(loopStart.set("loop start", 0, 0, 100));
    parameterGroup.add(loopEnd.set("loop end", 0, 0, 100));
    parameterGroup.add(looping.set("looping", false));
    parameterGroup.add(transpose.set("transpose", 0, -48, 48));
    parameterGroup.add(detune.set("detune", 0, -50, 50));
    parameterGroup.add(warp.set("warp", false));
}

void ofxAbletonLiveClip::play()
{
    ofxOscMessage msg;
    msg.setAddress("/live/play/clip");
    msg.addIntArg(track);
    msg.addIntArg(clip);
    sender->sendMessage(msg);
}

void ofxAbletonLiveClip::stop()
{
    ofxOscMessage msg;
    msg.setAddress("/live/stop/clip");
    msg.addIntArg(track);
    msg.addIntArg(clip);
    sender->sendMessage(msg);
}

void ofxAbletonLiveClip::setLoopStart(float loopStart)
{
    this->loopStart = loopStart;
    ofxOscMessage msg;
    msg.setAddress("/live/clip/loopstart");
    msg.addIntArg(track);
    msg.addIntArg(clip);
    msg.addFloatArg(loopStart);
    sender->sendMessage(msg);
}

void ofxAbletonLiveClip::setLoopEnd(float loopEnd)
{
    this->loopEnd = loopEnd;
    ofxOscMessage msg;
    msg.setAddress("/live/clip/loopend");
    msg.addIntArg(track);
    msg.addIntArg(clip);
    msg.addFloatArg(loopEnd);
    sender->sendMessage(msg);
}

void ofxAbletonLiveClip::setLooping(bool looping)
{
    this->looping = looping;
    ofxOscMessage msg;
    msg.setAddress("/live/clip/loopstate");
    msg.addIntArg(track);
    msg.addIntArg(clip);
    msg.addIntArg(looping ? 1 : 0);
    sender->sendMessage(msg);
}

void ofxAbletonLiveClip::setTranspose(int transpose)
{
    this->transpose = transpose;
    sendTransposeDetune();
}

void ofxAbletonLiveClip::setDetune(int detune)
{
    this->detune = detune;
    sendTransposeDetune();
}

void ofxAbletonLiveClip::setWarp(bool warp)
{
    this->warp = warp;
    ofxOscMessage msg;
    msg.setAddress("/live/clip/warping");
    msg.addIntArg(track);
    msg.addIntArg(clip);
    msg.addIntArg(warp ? 1 : 0);
    sender->sendMessage(msg);
}

void ofxAbletonLiveClip::sendTransposeDetune()
{
    ofxOscMessage msg;
    msg.setAddress("/live/pitch");
    msg.addIntArg(track);
    msg.addIntArg(clip);
    msg.addIntArg(transpose);
    msg.addIntArg(detune);
    sender->sendMessage(msg);
}