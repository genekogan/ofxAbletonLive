#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // can connect to AbletonLive in localhost (same machine) or over the network if an IP address given.
    // second argument is whether or not to setup a GUI
    live.setup("localhost");
    
    // it takes a couple of seconds to parse all the OSC data and build the gui.
    // wait a few seconds before calling any of the getter functions for it to populate the data
}

//--------------------------------------------------------------
void ofApp::update(){
    live.update();
    sequencerGlobal.update();
    sequencerDevice.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    sequencerGlobal.draw();
    sequencerDevice.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' ')
    {
        // global sequencer
        sequencerGlobal.setup(8);
        sequencerGlobal.setPosition(10, 10, 400, 200);
        sequencerGlobal.setSmooth(true);
        sequencerGlobal.addRow(&live.getTempo());
        sequencerGlobal.randomize();
        //sequencerGlobal.start();
        
        
        // device sequencer
        sequencerDevice.setup(8);
        sequencerDevice.setPosition(10, 250, 400, 500);
        sequencerDevice.setSmooth(true);

        ofxAbletonLiveDevice *device = live.getTrack("2-Audio")->getDevice("Vocoder");
        map<int,ofxAbletonLiveParameter*>::iterator it = device->getParameters().begin();
        for (; it != device->getParameters().end(); ++it) {
            sequencerDevice.addRow(it->second->getParameter());
        }
        
        sequencerDevice.randomize();
        sequencerDevice.start();
        
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
