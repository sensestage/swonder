// Virtual Panning sources:

// this means, we create multiple sources at different positions, and pan between them.
// we can then move these sources around as a group of sources.
// NOTE : The GUI does not support yet mouse control over group positions.

// adding some extra sources:
w.addSource( s.myOuts[2], (-3)@(-4), color: q.clientColors.at( q.myClientID ) );
w.addSource( s.myOuts[3], (0)@(-5), color: q.clientColors.at( q.myClientID ) );
w.addSource( s.myOuts[4], (0)@(0), color: q.clientColors.at( q.myClientID ) );

// creating a group, this is only local to your machine:
w.addGroup( 1, 0@(-2) );

// adding sources to the group:
w.addSourceToGroup( s.myOuts[0], 1 );
w.addSourceToGroup( s.myOuts[1], 1 );
w.addSourceToGroup( s.myOuts[2], 1 );
w.addSourceToGroup( s.myOuts[3], 1 );
w.addSourceToGroup( s.myOuts[4], 1 );

// moving the group:
w.groupPosition( 1, (-5)@(-10) );

// put the sources at random positions around the group position:
(0..5).do{ |it| w.sourceRelPosition( s.myOuts[it], (3.0.rand2 @ 3.0.rand2 ) ) };

(
x.free;
// a multichannel granular structure:
x = SynthDef( \multichanGrain, { | out = 0, carrier=500, modf= 300, fmi=1, amp=0.1|

	Out.ar( out, GrainFM.ar( 5, Dust.ar( 100 ), LFNoise1.kr( 2.0, 0.01, 0.02 ), carrier, modf, fmi, LFNoise1.kr( 2.0, 2 ) ) * amp
	);
}).play( s, [\out,s.myOuts[0], \carrier, 1000, \modf, 200, \fmi, 2 ] )

);

(
// moving the group around:
Tdef(\groupPend, {
	inf.do { |i|
		var xpos = (i * 0.02).sin * 20;
		var ypos = (i * 0.022).sin * 25 - 5;
		w.groupPosition( 1, xpos@ypos, 0.1 );
		0.1.wait;
	};
}).play;
);

(
// creating movement within the group:
Tdef(\innerGroupMovement, {
	inf.do { |i|
		// put the sources at random positions around the group position:
		(0..4).do{ |it| w.sourceRelPosition( s.myOuts[it], (2.0.rand2 @ 2.0.rand2 ), 0.5 ) };
		0.5.wait;
	};
}).play;
);



(
x.free;
// another sound example:
x = SynthDef( \multichanNoisy, { | out = 0, freq = 800, rq = 0.05, drate = 10, ddur=0.2, amp=0.1|

	Out.ar( out, PanAz.ar( 5, 
		(BPF.ar( PinkNoise.ar( 20 ), freq * [1,1.4,1.8] + LFNoise1.kr( drate, freq/8 ), rq ) * amp * [1,0.8,0.4]),
		SinOsc.kr( [10,12,14] ), 3
	).sum );
}).play( s, [\out,s.myOuts[0], \freq, 1000 ] );

)

x.set( \freq, 800 )
x.set( \rq, 0.01 )

x.free;

Tdef( \innerGroupMovement ).stop;
Tdef( \groupPend ).stop;