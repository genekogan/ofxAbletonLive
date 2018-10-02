ofxAbletonLive is an addon for openFrameworks which controls [Ableton Live](https://www.ableton.com) from an OF app, allowing remote control without MaxForLive. Communication is handled over OSC using the [LiveOSC plugin](https://livecontrol.q3f.org/ableton-liveapi/liveosc/) and thus can be done across a network. Communication is 2-way and thus parameters remain in sync whether modified from the app or from Ableton.

Most of Ableton's main features are implemented, but not all. A list of the OSC commands available can be [found here](https://github.com/hanshuebner/LiveOSC/blob/master/OSCAPI.txt). Most have been wrapped.

## Installation

* [Download LiveOSC from the releases page](https://github.com/genekogan/ofxAbletonLive/releases/download/v1.0/LiveOSC.zip).

* Place the "LiveOSC" folder (inside "trunk") into the Midi Remote Scripts folder in your Live App.
 * On OSX, go to your applications folder, right click Ableton and select "show package contents", then navigate to `Contents/App-Resources/MIDI Remote Scripts` and paste there.
 * On Windows, paste into `\path\to\ableton\Live x.x.x\Resources\MIDI\Remote Scripts`

* Restart Ableton. Go to `preferences` -> `MIDI`. LiveOSC should show up in the drop-down menu under "Control Surface". Select it.

* Try the examples. Live communicates with your app over ports `9000` and `9001`, so make sure those are not occupied (including by another instance of ofxAbletonLive)


#### Fix for Ableton 9.6+

If LiveOSC fails to work in most recent version of Ableton, [this apparently fixes it](http://disq.us/p/15j82c7) -- find the file `socket_live8.py` and comment out the following block of code (lines 74-79):

    # if _have_ssl:
    # _realssl = ssl
    # def ssl(sock, keyfile=None, certfile=None):
    # if hasattr(sock, "_sock"):
    # sock = sock._sock
    # return _realssl(sock, keyfile, certfile)


## Usage

The main class is `ofxAbletonLive`. An instance of `ofxAbletonLive` contains a collection of `ofxAbletonLiveTrack`. An `ofxAbletonLiveTrack` instance contains a collection of `ofxAbletonLiveClip` and `ofxAbletonLiveDevice`. `ofxAbletonDevice` contains a collection of `ofxAbletonLiveParameter`. All classes have various properties that can be set remotely.

Connect to Ableton:

    ofxAbletonLive live;
    live.setup();

By default, the app connects to Live over localhost. You can also connect over the network to a machine running Live by including its IP address, e.g.

    live.setup("192.168.1.2")

Optionally you can pass a callback method to be called once ofxAbletonLive is finished parsing the current live set (after setup has been called). The callback method has no arguments.

    //void ofApp::myCallback() {}

    live.setup(this, &ofApp::myCallback);
    live.setup("192.168.1.2", this, &ofApp::myCallback);

Note also that ofxAbletonLive handles communication with Ableton through ports 9000 and 9001. Make sure these ports are not occupied if you are getting no results (including through another instance of ofxAbletonLive).

There are getters and setters for global variables in Live.

    live.setTempo(120);
	float tempo = live.getTempo();
	
	live.setTime(8.0);
    float time = live.getTime();
	
    live.setOverdub(true);
    bool overdub = live.getOverdub();
	
    live.setVolume(1.0);  // master volume, 0->1
    float volume = live.getVolume();
	
    live.setPan(0.0);	 // master pan, -1->1
    float pan = live.getPan();
	
	live.setQuantization(0);	// 0 = 8 bars, 1 = 4 bars ... 13 = 1/32nd bar
	
	live.setCrossFader(0.5);
	float crossfade = live.getCrossFader();
	
	int numTracks = live.getNumTracks();
	int numScenes = live.getNumScenes();

Playback commands	

	live.play();			// play from beginning
	live.playContinue();	// play from current time
	live.playSelection();	// play selection
	live.stop();
	
	live.prevCue();			// jump to previous cue point (global, not track)
	live.nextCue();			// jump to previous cue point (global, not track)
	
	live.selectScene(2);	// select a scene (global)
	
Working with individual tracks

	// we can get info about our tracks
	live.printTracks();
	
	// or have it spit out everything we know about the live set
	live.printAll();

	// from the track info printed above we find out the index and name of each track
	
	// grab a reference to a track by index
    ofxAbletonLiveTrack *track = live.getTrack(0);

	// or by name...
	// careful! either method returns NULL if track doesn't exist
    ofxAbletonLiveTrack *track = live.getTrack("My Track");
    
    // tracks have these setters (and the corresponding getters)
    track->setVolume(ofRandom(1));
    track->setPan(ofRandom(-1, 1));
    track->setArmed(false);
    track->setMute(false);
    track->setSolo(false);

Working with a track's clips

	// get a track's clips
	ofxAbletonLiveTrack *track = live.getTrack(0);
	track->printClips();

	// grab a reference to a clip, either by index or by name
	// careful! either method returns NULL if clip doesn't exist
	ofxAbletonLiveClip *clip = track->getClip(0);
	ofxAbletonLiveClip *clip = track->getClip("Name of my clip");

	// clip actions
	clip->play();
	clip->setDetune(-25);  // -50 to 50
	clip->setTranspose(36);  // -48 to 48
	clip->setLooping(true);
	clip->setLoopStart(25.0);
	clip->setLoopEnd(100.0);
	clip->setWarp(true);

Working with a track's devices

	ofxAbletonLiveTrack *track = live.getTrack(0);

	// print all the devices we have (index, name)
	track->printDevices();

	// grab a reference to a device, either by index or by name
	// careful! either method returns NULL if clip doesn't exist
	ofxAbletonLiveDevice *device = track->getDevice(0);
	ofxAbletonLiveDevice *device = track->getDevice("Name of my clip");

	// you can enable/disable devices
	device->setEnabled(false);
	
	// you can query a device for its parameters
	// returns: (index) "name" {min, max}
	device->printParameters();
    
    // get a handle to a parameter by index or name (note: index start at 1, not 0)
	// careful! either method returns NULL if clip doesn't exist
    ofxAbletonLiveParameter *parameter = device->getParameter(1);
    ofxAbletonLiveParameter *parameter = device->getParameter("Attack");
    
    // set value of parameter by hand
    parameter->setValue(0.5);

    // you can chain the getters to grab the desired parameter, e.g
	live.getTrack("2-Audio")->getDevice("Vocoder")->getParameter("Dry/Wet")->setValue(0.7);
	
## Included examples

 - example: basic example showing how to get info from `ofxAbletonLive` and most of the commands listed above
 - example_gui: added convenience class `AbletonGui` which generates an `ofxPanel` from ofxAbletonLive
 - example_sequencer: (requires [ofxSequencer](https://www.github.com/genekogan/ofxSequencer)) controlling Ableton parameters with a sequencer

## Known bugs, missing features, to-do

 - LiveOSC does not distinguish any of the features by type (int, float) so all parameters are handled as floats -- including even menus, i.e. a device parameter which is a menu can be set by sending it a float corresponding to the index of the choice you want to select. Only exception is the "Device On" parameter which is hardcoded as a boolean.
 - LiveOSC doesn't expose all of Ableton's features -- it basically makes accessible anything that is MIDI-controllable. So you can't load clips/devices on the fly or change some of the properties of the clips
 - To-do: Handle more incoming messages (beats)

