(
// Creating the (last) 8 sources as 8 plane waves from different directions:
// in the workshop only one of us needs to execute this
s.reverbChannels.do{ |i| var angle = (i-s.reverbChannels[0])*45; 
w.addSource( i,
	Point( *([ cos( angle*pi/180 ), -1*sin( angle*pi / 180 ) ] * 20 + [0,13]) ) );
w.sourcePlane(i,true);
w.sourceAngle(i, (angle+180)* -1 ); 
w.sourceColor( i, Color.black )
};
);

// Examples:

(
x.free; y.free;
// some sound we will reverberize
x = SynthDef( \src, { Out.ar( s.myOuts[0], Decay.ar(Impulse.ar(1), 0.25, LFCub.ar(1200,0,0.1))  ) }).play;
y = SynthDef( \src2, { Out.ar( s.myOuts[1], Decay.ar(Impulse.ar(0.3), 0.25, LFCub.ar(1300,0,0.1))  ) }).play;
);

(
// Mono input source, using FreeVerb, duplicating it to all 8 plane waves
z.free;
z = SynthDef(\rev1, {|in=0, out=32,room = 0.15, damp = 0.5, amp=1|
	Out.ar(out, amp*
		FreeVerb.ar(
			In.ar( in, 1), // mono src
			1, // mix 0-1
			room, // room 0-1
			damp // damp 0-1 duh
		) ! 8 //fan out...
	);
}).play( s, [\in, s.myOuts[0], \out, s.reverbChannels[0] ], \addToTail )
);

// change some parameters of FreeVerb
z.set( \room, 0.5 )
z.set( \damp, 0.8 )
z.set( \amp, 0.8 )


(
// stereo freeverb with mono input to 8 plane waves
z.free;
z = SynthDef(\rev2, {|in=0, out=32, room = 0.15, damp = 0.5, amp=1|
	var src = In.ar( in, 1 );
	Out.ar(out, amp*
		( FreeVerb2.ar(
			src, src, 
			1, // mix 0-1
			room, // room 0-1
			damp // damp 0-1 duh
		) ! 4 ).flatten //fan out...
	);
}).play( s, [\in, s.myOuts[0], \out, s.reverbChannels[0] ], \addToTail )
);

// change some parameters of FreeVerb
z.set( \room, 0.7 )
z.set( \damp, 0.3 )
z.set( \amp, 0.8 )


// stereo source to freeverb2
(
z.free;
z = SynthDef(\rev3, {|in=0, out=32, room = 0.15, damp = 0.5, amp=1|
	var src = In.ar( in, 2 );
	Out.ar(out, amp*
		( FreeVerb2.ar(
			src[0], src[1], 
			1, // mix 0-1
			room, // room 0-1
			damp // damp 0-1 duh
		) ! 4 ).flatten //fan out...
	);
}).play( s, [\in, s.myOuts[0], \out, s.reverbChannels[0] ], \addToTail )
);

// change some parameters of FreeVerb
z.set( \room, 0.8 )
z.set( \damp, 0.1 )
z.set( \amp, 0.8 )


/// comb based reverb:

(
SynthDef( \combreverb, { arg out=0, in=0, amp=1, lag=1, lagamp=0.1, lagdecay=0.1, lagdelay=0.1, gate=1, delay=1.0, maxcdelay = 2.0;
	var envcut, input, amps, combs, delays, decays;
	amps =   Control.names([\amps]).kr(   Array.fill(8, 0.5 ));
	delays = Control.names([\delays]).kr( Array.fill( 8, { 0.5.rand + 0.1} ) );
	decays = Control.names([\decays]).kr( Array.fill( 8, { 2.0.rand + 1.0} ) );
	
	input = DelayL.ar( In.ar( in, 1 ), 5.0, delay.clip(0.0,5.0) );
	//	inputs = input * amps.lag(lagamp);
	combs = Array.fill( 8, 0 );
	combs = CombL.ar( input, maxcdelay, delays.lag(lagdelay) + LFNoise1.kr( 0.2, 0.05), decays.lag( lagdelay) + LFNoise1.kr( 0.2, 0.05), amps.lag(lagamp) );
	envcut = EnvGen.kr( Env.cutoff(1,1,4), gate, doneAction: 2 );
	Out.ar( out, combs * amp.lag(lag) * envcut );
}).send(s);
)

(
z.free;
z = Synth.new( \combreverb, [\in, s.myOuts[0], \out, s.reverbChannels[0] ], s, \addToTail )
);

z.set( \delay, 0.01 ); // time that the sound is delayed before the reverb
z.set( \amp, 1 ); // amplitude of the reverberation
z.setn( \delays, Array.fill( 8, { 0.4.rand + 0.1} ) ); // reverberation/comb delay times
z.setn( \decays, Array.fill( 8, { 2.0.rand + 2.0} ) ); // comb decay times
z.setn( \amps, [ 0.3, 0.23, 0.13, 0.13, 0.13, 0.13, 0.13, 0.13 ]*0.5 ); // amplitudes of each direction


(
SynthDef( \combreverb2, { arg out=0, in=0, amp=1, lag=1, lagamp=0.1, lagdecay=0.1, lagdelay=0.1, gate=1, delay=1.0, maxcdelay = 2.0, cdelay=1.0, cdecay = 2;
	var envcut, input, amps, combs, delays, decays;
	amps =   Control.names([\amps]).kr(   Array.fill(8, 0.5 ));
	delays = Control.names([\delays]).kr( Array.fill( 8, { 0.5.rand + 0.1} ) );
	decays = Control.names([\decays]).kr( Array.fill( 8, { 2.0.rand + 1.0} ) );
	
	input = DelayL.ar( In.ar( in, 1 ), 5.0, delay.clip(0.0,5.0) );
	//	inputs = input * amps.lag(lagamp);

	combs = CombL.ar( input, maxcdelay, delays.lag(lagdelay) + LFNoise1.kr( 0.2, 0.05), decays.lag( lagdelay) + LFNoise1.kr( 0.2, 0.05), amps.lag(lagamp) );
	3.do{
		combs = CombL.ar( combs, maxcdelay, cdelay + LFNoise1.kr(0.3,0.05), cdecay + LFNoise1.kr( 0.2, 0.3) );
	};
	envcut = EnvGen.kr( Env.cutoff(1,1,4), gate, doneAction: 2 );
	Out.ar( out, combs * amp.lag(lag) * envcut );
}).send(s);
)

(
z.free;
z = Synth.new( \combreverb2, [\in, s.myOuts[0], \out, s.reverbChannels[0] ], s, \addToTail )
);

z.set( \delay, 0.01 ); // time that the sound is delayed before the reverb
z.set( \amp, 1 ); // amplitude of the reverberation
z.setn( \delays, Array.fill( 8, { 0.4.rand + 0.1} ) ); // reverberation/comb delay times
z.setn( \decays, Array.fill( 8, { 2.0.rand + 2.0} ) ); // comb decay times
z.setn( \amps, [ 0.3, 0.23, 0.13, 0.13, 0.13, 0.13, 0.13, 0.13 ]*0.5 ); // amplitudes of each direction
z.set( \cdelay, 0.3)
z.set( \cdecay, 3 )


z.free;
x.free; y.free;

/// you could change the amplitude of each direction, based on the current position of the sound source