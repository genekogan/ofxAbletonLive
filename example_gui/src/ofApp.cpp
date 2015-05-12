#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // can connect to AbletonLive in localhost (same machine) or over the network if an IP address given.
    // additionally provide an event listener for when live has been setup
    live.setup("localhost", this, &ofApp::setupAbletonGui);
    
    // it takes a couple of seconds to parse all the OSC data and build the gui.
    // wait a few seconds before calling any of the getter functions for it to populate the data
    
}

//--------------------------------------------------------------
void ofApp::update(){
    // don't forget to run this! otherwise no OSC messages received
    live.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::setupAbletonGui()
{
    // setupAbletonGui() was passed to the live setup method and will
    // automatically be called when live has finished parsing. at that
    // point we can generate a gui
    gui.setup(&live);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    
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
