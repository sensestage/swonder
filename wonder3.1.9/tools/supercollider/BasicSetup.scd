// =====================================================================
// WFS workshop - basic setup
// =====================================================================

// Setting up a connection with the WONDER software by using the SWonder class:
w = SWonder( NetAddr("192.168.3.254", 58100)); // creating an instance of SWonder
//w.debug_(false);
w.connect; // connect it to cwonder

// give it a little bit of time to get all the positions:

w.drawGUI_(true); // draw a nice GUI

// Project management
// WONDER needs to have a project loaded, so we create one here.
// Since we are all talking to the same WONDER, only one of us needs to do this for this workshop,
// if you are on your own, you always need to create a project, or load a previous one
w.createProject( "SC-workshop" ); // create a new project

w.loadProject( "SC-workshop" ); // load project if it already existed

w.saveProject( "SC-workshop" ); // save project under a specific name


/// we will all be playing our sounds on the instance of scsynth that runs on the
/// control computer of the WFS system. We will use a SharedServer, so we have
/// control over the server over the stuff that we create, without destroying things from
/// others.

(
q = q ? ();

q.myClientID = 1; // CHANGE FOR YOURS!!
q.numPlayers = 2; // agree on a maximum number

// prepare a wfs shared server:
q.options = WFSSharedServerOptions.new
.numAudioBusChannels_(1024)
.numInputBusChannels_(8)
.numOutputBusChannels_(42)
.numReverbChannels_( 8 ); // reverb channels are used for sources which are plane wave sources in the WFS system, from 8 different directions, on which we can play reverberation kind of sounds.

q.numChans = q.options.numOutputBusChannels;
q.wfs = WFSSharedServer(\wfs, NetAddr("192.168.3.100", 57110), q.options, q.myClientID);
//q.wfs = WFSSharedServer(\wfs, NetAddr("127.0.0.1", 57110), q.options, q.myClientID);
Server.default = s = q.wfs;

CmdPeriod.add({ q.wfs.freeAll }); // frees only my own nodes, not all of them
q.wfs.initTree;
q.wfs.numClients_(q.numPlayers);
"I can play on chans: %\n".postf(q.wfs.myOuts);
"The reverb channels are: %\n".postf(q.wfs.reverbChannels);
);

(
/// CREATING a GUI for the server with nice buttons to kill our sound:
try { q.wfsWin.close };
q.wfsWin = Window("WFS server", Rect(0,800,300, 150)).userCanClose_(false).front;
q.wfsWin.addFlowLayout;

Button(q.wfsWin, Rect(0,0, 140, 50))
.states_([["EMERGENCY STOP", Color.white, Color.red]])
.action_({ q.wfs.freeAll(true); });

Button(q.wfsWin, Rect(0,150, 140, 50))
.states_([["Free All \nMy Sounds.", Color.white, Color.green]])
.action_({ q.wfs.freeAll });

q.wfs.makeGui(q.wfsWin);
);

// ------- testing the setup ---------

w.addSource( s.myOuts[0], (-5@(-2) ) );

// a simple test sound, playing to s.myOuts, channel 0
x.free; x = { Ringz.ar(Dust.ar([30, 20, 10]), [600, 1800, 7200], 0.04).sum * 0.1 }.play(s, s.myOuts[0]);

w.sourcePosition( s.myOuts[0], (5)@(-2) );
w.sourcePosition( s.myOuts[0], (-5)@(-2) );

w.addSource( s.myOuts[1], (5@(-2)) ); 

// channel 1
x.free; x = { Ringz.ar(Dust.ar([30, 20, 10]), [600, 1800, 7200], 0.04).sum * 0.1 }.play(s, s.myOuts[1]);

x.free; x = { Ringz.ar( WhiteNoise.ar( 0.3), [600, 1800, 7200], 0.04).sum * 0.03 }.play(s, s.myOuts[1]);

// free it
x.free;

// EMERGENCY:
q.wfs.freeAll; // kill your own sounds
q.wfs.freeAll(true); // kill everything on the server.
