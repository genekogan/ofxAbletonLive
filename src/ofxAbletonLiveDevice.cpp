#include "ofxAbletonLiveDevice.h"


ofxAbletonLiveDevice::ofxAbletonLiveDevice(string name, int track, int device, ofxOscSender *sender)
{
    this->name = name;
    this->track = track;
    this->device = device;
    this->sender = sender;
    initialized = false;
    enabled.set("Device On", false);
    enabled.addListener(this, &ofxAbletonLiveDevice::eventSetEnabled);
    parameterGroup = new ofParameterGroup();
    parameterGroup->setName(name);
    parameterGroup->add(enabled);
}

void ofxAbletonLiveDevice::clear()
{
    map<int, ofxAbletonLiveParameter*>::iterator it = parameters.begin();
    for (; it != parameters.end(); ++it)
    {
        delete it->second;
        parameters.erase(it);
    }
    parameters.clear();
    parametersLU.clear();
    parameterGroup->clear();
}

void ofxAbletonLiveDevice::addParameter(int index, string name, float value, bool isReturn)
{
    if (!parameterGroup->contains(name) && name != "Device On") // skip "Device On" which is handled by separate ofParameter
    {
        ofParameter<float> *newParameter = new ofParameter<float>(name, value);
        ofxAbletonLiveParameter *newLiveParameter = new ofxAbletonLiveParameter(newParameter, track, device, index, sender);
        parameters[index] = newLiveParameter;
        parametersLU[name] = parameters[index];
        parameterGroup->add(*newParameter);
        if (isReturn) {
            newLiveParameter->setOscAddress("/live/return/device");
        }
    }
}

void ofxAbletonLiveDevice::setEnabled(bool enabled)
{
    this->enabled = enabled;
    ofxOscMessage msg;
    msg.setAddress("/live/device");
    msg.addIntArg(track);
    msg.addIntArg(device);
    msg.addIntArg(0);
    msg.addIntArg(enabled ? 1 : 0);
    sender->sendMessage(msg);
}

void ofxAbletonLiveDevice::receiveParameterValues(ofxOscMessage &m, bool isReturn)
{
    for (int i = 2; i < m.getNumArgs(); i+=3)
    {
        int index = m.getArgAsInt32(i);
        float value = m.getArgAsFloat(i+1);
        string name = m.getArgAsString(i+2);
        addParameter(index, name, value, isReturn);
    }
}

void ofxAbletonLiveDevice::receiveParameterRanges(ofxOscMessage &m)
{
    for (int i = 2; i < m.getNumArgs(); i+=3)
    {
        int index = m.getArgAsInt32(i);
        float min = m.getArgAsFloat(i+1);
        float max = m.getArgAsFloat(i+2);
        if (parameters.count(index) > 0)
        {
            parameters[index]->getParameter()->setMin(min);
            parameters[index]->getParameter()->setMax(max);
        }
    }
    initialized = true;
}

ofxAbletonLiveParameter * ofxAbletonLiveDevice::getParameter(int index)
{
    if (parameters.count(index) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access parameter which does not exist!");
        return NULL;
    }
    else {
        return parameters[index];
    }
}

ofxAbletonLiveParameter * ofxAbletonLiveDevice::getParameter(string name)
{
    if (parametersLU.count(name) == 0)
    {
        ofLog(OF_LOG_WARNING, "Trying to access parameter which does not exist!");
        return NULL;
    }
    else {
        return parametersLU[name];
    }
}

string ofxAbletonLiveDevice::getParametersInfo()
{
    string s;
    map<int, ofxAbletonLiveParameter*>::iterator it = parameters.begin();
    s += "Parameters for Track " + ofToString(track) + " Device "+ ofToString(device) +": \n";
    for (; it != parameters.end(); ++it) {
        s += (" -> ("+ ofToString(it->first) +") \"" + it->second->getName() + "\"  {"+ofToString(it->second->getMin())+", "+ofToString(it->second->getMax())+"}\n");
    }
    return s;
}

ofxAbletonLiveDevice::~ofxAbletonLiveDevice()
{
    enabled.removeListener(this, &ofxAbletonLiveDevice::eventSetEnabled);
    clear();
    delete parameterGroup;
}
