#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // by default, connect to AbletonLive on local machine
    live.setup();
    
    // can also connect over the network if IP address specified, e.g.
    //live.setup("192.168.1.15");
    
    // lastly you can provide a callback method (with no arguments)
    // to be called once live has finished parsing, as in the following:
    //live.setup(this, &ofApp::myCallbackMethod);
    //live.setup("192.168.1.15", this, &ofApp::myCallbackMethod);
    //live.setup("localhost", this, &ofApp::myCallbackMethod);
    
    // ofxAbletonLive takes a few seconds to get all the data over OSC from Ableton.
    // during this time, parameters aren't visible yet or accessible.
    // you can check the status by calling live.isLoaded(), or as described above,
    // you can pass a callback method to the setup
}

//--------------------------------------------------------------
void ofApp::update(){
    // don't forget to run this! otherwise no OSC messages will be received
    live.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0);
    
    // this draw method prints all of the relevant info from ofxAbletonLive
    // you can also call live.drawDebugView() to do the same
    
    // you can check if Live has finished loading all parameters since setup was called
    if (!live.isLoaded()) {
        ofDrawBitmapString("ofxAbletonLive has not loaded yet!", 100, 100);
        return;
    }

    // now that live is fully loaded, we can print all the parameters to the screen
    
    ofDrawBitmapString(live.getTrackString(), 10, 15);
    int x = 200;
    
    map<int, ofxAbletonLiveTrack*>::iterator it = live.getTracks().begin();
    for (; it != live.getTracks().end(); ++it)
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    // get info
    if (key=='1')
    {
        // you can print all the available parameters to the console
        live.printAll();
        
        // or just the tracks
        live.printTracks();
    }

    // live's global properties
    else if (key=='2')
    {
        live.setTempo(88);
        live.setTime(20);
        live.setOverdub(true);
        live.setPan(-0.2);
        live.setVolume(0.8);
        live.setQuantization(4); // 0 = 8 bars, 1 = 4 bars ... 13 = 1/32nd bar
        live.setCrossFader(0.5);
    }
    
    // global playback controls
    else if (key=='3')
    {
        live.play();
        live.playContinue();
        live.playSelection();
        live.stop();
        
        live.prevCue();
        live.nextCue();
        
        live.selectScene(2);    // select 3rd row of clips
    }

    // working with track properties
    else if (key=='4')
    {
        // can get a handle to a track by index or by name.
        // the name and index correspond to what's in live.printTracks().
        // careful! returns NULL if track doesn't exist
        ofxAbletonLiveTrack *trackByIndex = live.getTrack(0);
        ofxAbletonLiveTrack *trackByName = live.getTrack("My Track");

        // tracks have a number of features with getters/setters
        trackByIndex->setVolume(ofRandom(1));
        trackByIndex->setPan(ofRandom(-1, 1));
        trackByIndex->setArmed(false);
        trackByIndex->setMute(false);
        trackByIndex->setSolo(false);
    }
    
    // get a track's clips
    else if (key=='5')
    {
        ofxAbletonLiveTrack *track = live.getTrack(1);
        
        // print all the clips we have (index, name)
        track->printClips();

        // grab a reference to a clip, either by index or by name
        ofxAbletonLiveClip *clipByIndex = track->getClip(0);
        ofxAbletonLiveClip *clipByName = track->getClip("Name of my clip");
        
        // clip actions
        clipByIndex->play();
        clipByIndex->setDetune(-25);  // -50 to 50
        clipByIndex->setTranspose(36);  // -48 to 48
        clipByIndex->setLooping(true);
        clipByIndex->setLoopStart(25.0);
        clipByIndex->setLoopEnd(100.0);
        clipByIndex->setWarp(true);
    }
    
    // get a track's devices
    else if (key=='6')
    {
        ofxAbletonLiveTrack *track = live.getTrack(0);
        
        // print all the devices we have (index, name)
        track->printDevices();

        // grab a reference to a device, either by index or by name
        ofxAbletonLiveDevice *deviceByIndex = track->getDevice(0);
        ofxAbletonLiveDevice *deviceByName = track->getDevice("Name of my clip");
        
        // you can enable/disable devices
        deviceByIndex->setEnabled(false);
    }
    
    // work with device parameters
    else if (key=='7')
    {
        ofxAbletonLiveTrack *track = live.getTrack(0);
        ofxAbletonLiveDevice *device = track->getDevice(0);
        
        // get all the parameters belonging to the device
        device->printParameters();
        
        // get a handle to a parameter by index (note: parameters start at 1, not 0) not name
        ofxAbletonLiveParameter *parameterByIndex = device->getParameter(1);
        ofxAbletonLiveParameter *parameterByName = device->getParameter("Attack");
        
        // set value of parameter by hand
        parameterByIndex->setValue(0.5);
    }
    
    // get a track's clips
    else if (key=='8')
    {
        ofxAbletonLiveTrack *track = live.getTrack(0);
        
        // print all the clips we have (index, name)
        track->printClips();
        
        // get clip by index or name
        ofxAbletonLiveClip *clipByIndex = track->getClip(0);
        ofxAbletonLiveClip *clipByName = track->getClip("my clip");
        
        // playback options for clips
        clipByIndex->play();
        clipByIndex->stop();
        clipByIndex->setLoopStart(1.0);
        clipByIndex->setLoopEnd(4.0);
        clipByIndex->setLooping(true);
        clipByIndex->setWarp(false);
        clipByIndex->setTranspose(10); // -48 to 48
        clipByIndex->setDetune(-20);  // -50 to 50
    }
    
    // refreshing ofxAbletonLive
    else if (key=='9')
    {
        // if you have made changes to your live set, refresh!
        live.refresh();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
