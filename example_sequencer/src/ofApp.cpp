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
}

//--------------------------------------------------------------
void ofApp::draw(){
    sequencerGlobal.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' ')
    {
        sequencerGlobal.setup(8);
        sequencerGlobal.setSmooth(true);
        sequencerGlobal.addRow(live.getTrack("2-Audio")->getDevice("Vocoder")->getParameter("Dry/Wet")->getParameter());
        sequencerGlobal.randomize();
        sequencerGlobal.start();
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
