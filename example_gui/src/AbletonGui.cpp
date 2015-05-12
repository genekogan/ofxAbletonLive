#include "AbletonGui.h"


AbletonGui::AbletonGuiTrack::AbletonGuiTrack(ofxAbletonLiveTrack *track)
{
    this->track = track;
    setup();
}

void AbletonGui::AbletonGuiTrack::setup()
{
    tVisible.setup(track->getName(), false);
    panel.setup(track->getName());

    // setup playback gui
    panelPlayback.setup("Playback");
    if (!track->isReturn()) panelPlayback.add(track->getArmed());
    panelPlayback.add(track->getMute());
    panelPlayback.add(track->getSolo());
    panelPlayback.add(track->getVolume());
    panelPlayback.add(track->getPan());
    panel.add(&panelPlayback);
    
    // setup sends gui
    panelSends.setup("Sends");
    map<int, ofxAbletonLiveTrack::ofxAbletonLiveSend*>::iterator its = track->getSends().begin();
    for (; its != track->getSends().end(); ++its) {
        panelSends.add(its->second->send);
    }
    panel.add(&panelSends);

    // setup clips gui
    if (!track->isReturn())
    {
        panelClips.setup("Clips");
        map<int, ofxAbletonLiveClip*>::iterator itc = track->getClips().begin();
        for (; itc != track->getClips().end(); ++itc) {
            panelClips.add(itc->second->getParameterGroup());
        }
        panel.add(&panelClips);
    }
    
    // setup device guis
    panelDevices.setup("Devices");
    map<int, ofxAbletonLiveDevice*>::iterator itd = track->getDevices().begin();
    for (; itd != track->getDevices().end(); ++itd) {
        panelDevices.add(*itd->second->getParameterGroup());
    }
    panel.add(&panelDevices);
}

void AbletonGui::setup(ofxAbletonLive * live)
{
    this->live = live;
    
    bPlay.addListener(live, &ofxAbletonLive::play);
    bContinue.addListener(live, &ofxAbletonLive::playContinue);
    bSelection.addListener(live, &ofxAbletonLive::playSelection);
    bStop.addListener(live, &ofxAbletonLive::stop);
    bPrev.addListener(live, &ofxAbletonLive::prevCue);
    bNext.addListener(live, &ofxAbletonLive::nextCue);
    
    guiGlobalParameters.setup("Global");
    guiGlobalParameters.add(live->getTempo());
    guiGlobalParameters.add(live->getTime());
    guiGlobalParameters.add(live->getOverdub());
    guiGlobalParameters.add(live->getVolume());
    guiGlobalParameters.add(live->getPan());
    guiGlobalParameters.add(live->getCrossFader());

    guiPlayback.setup("Playback");
    guiPlayback.add(bPlay.setup("play", false));
    guiPlayback.add(bContinue.setup("continue", false));
    guiPlayback.add(bSelection.setup("play selection", false));
    guiPlayback.add(bStop.setup("stop", false));
    guiPlayback.add(bPrev.setup("prev cue", false));
    guiPlayback.add(bNext.setup("next cue", false));
    
    guiSelectActiveTrack.setup("View Tracks");

    cout << "TRACK !" <<live->getTracks().size()<<endl;
    
    map<int, ofxAbletonLiveTrack*>::iterator it = live->getTracks().begin();
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = live->getReturnTracks().begin();
    for (; it != live->getTracks().end(); ++it)
    {
        cout << "yo " << endl;
        AbletonGuiTrack *newTrack = new AbletonGuiTrack(it->second);
        tracks.push_back(newTrack);
        newTrack->tVisible.addListener(this, &AbletonGui::eventSelectTrack);
        guiSelectActiveTrack.add(&newTrack->tVisible);
    }
    for (; itr != live->getReturnTracks().end(); ++itr)
    {
        cout << "yo 2" << endl;
        AbletonGuiTrack *newTrack = new AbletonGuiTrack(itr->second);
        tracks.push_back(newTrack);
        newTrack->tVisible.addListener(this, &AbletonGui::eventSelectTrack);
        guiSelectActiveTrack.add(&newTrack->tVisible);
    }
    
    gui.setup("AbletonLive");
    gui.setPosition(0, 0);
    gui.add(&guiGlobalParameters);
    gui.add(&guiPlayback);
    gui.add(&guiSelectActiveTrack);
}

void AbletonGui::eventSelectTrack(bool & b)
{
    ofPoint position = ofPoint(gui.getShape().x + gui.getShape().width + 4, gui.getShape().y);
    vector<AbletonGuiTrack*>::iterator it = tracks.begin();
    for (; it != tracks.end(); ++it)
    {
        if ((*it)->tVisible)
        {
            (*it)->panel.setPosition(position);
            position = ofPoint((*it)->panel.getShape().x + (*it)->panel.getShape().width + 4, (*it)->panel.getShape().y);
        }
    }
}

void AbletonGui::draw()
{
    if (live == NULL) return;

    gui.draw();
    vector<AbletonGuiTrack*>::iterator it = tracks.begin();
    for (; it != tracks.end(); ++it)
    {
        if ((*it)->tVisible) {
            (*it)->panel.draw();
        }
    }

}