## AbletonOSC

This application presents an interface for choosing parameters from your Ableton set and routing OSC messages to them. It is meant to live in between your OSC-sending application and Ableton, facilitating the process of latching parameters without having to derive their addresses yourself (which is somewhat tedious using LiveOSC).

More detailed instructions and a tutorial will be posted soon. For now, see these bare-bones instructions:

#### Instructions

1) Install and activate [LiveOSC](http://livecontrol.q3f.org/ableton-liveapi/liveosc/) in Ableton.

2) Run this application (requires ofxAbletonLive and ofxControl or download the [release](https://github.com/genekogan/ofxAbletonLive/releases)).

3) Launch Ableton, and then launch AbletonOSC. AbletonOSC will then parse your current Ableton Live set and return all the available parameters, at which point you can select them from your gui.

4) To control the selected parameters, see the output address in the OSC gui (by default `/wek/outputs`) and port (default 12000).  Sending a multi-value array of parameter values (between 0 and 1) to this address on localhost will change the values. Alternatively, you can change one parameter at a time by sending to `/wek/outputs/#` where \# is the index of the parameter.

5) At this point, changing the outputs in your application should change them in Ableton via AbletonOSC.

6) ♩♪♫♪