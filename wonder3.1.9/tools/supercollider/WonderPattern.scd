// =====================================================================
// WFS Workshop Pattern example
// =====================================================================

// w contains our reference to SWonder

// example short percussive sound:
(
SynthDef(\pgrain,
	{ arg out = 0, freq=800, sustain=0.01, amp=0.5, pan = 0;
		var window;
		window = Env.sine(sustain, amp * AmpCompA.kr(freq));
		Out.ar(out,
			BPF.ar( PinkNoise.ar(100), freq, 0.02)
			* EnvGen.ar(window, doneAction:2)
		)
	}
).add;
);

// create a GUI for our patterns:
PdefAllGui.new;

(
// this is the pattern that will send the source position to SWonder
Pdef( \toWonder, 
	Pbind( 
		\srcid, Pkey( \out ), // source id number is assumed to be equal to the output channel
		\pos, Pfunc( { |inval| if ( inval.pos.isNil ){ [inval.posx,inval.posy] }{ inval.pos } } ), 
		// position is equal to either the given pos in the input event (should be an array of size 2),
		// or it is equal to an array made from the keys posx and posy
		\toWFS, Pfunc( { |inval| w.sourcePosition( inval.srcid, Point( *inval.pos )  ); [inval.srcid, inval.pos ];  } )
		// this function sends the data to the instance of SWonder
	)
);
);

// this will make our sound and create positions:
Pdef( \tick, Pbind( \degree, Pxrand([0,3,5,3],inf), \octave, Pseq([7,6],inf), \amp, [0.1,0.1]*2, \dur, 0.1, \delta, 0.3, \instrument, \pgrain, \out, Pseq( s.myOuts.at( (0..2) ), inf ), \posx, Prand([1,2,3]*12 -24,inf), \posy, Prand([-1,-2,0,1,2,3,4]*10,inf) )  );



// this is chaining the two patterns
Pdef( \tickToWonder, (Pdef(\toWonder) <> Pdef(\tick)) );

// and playing the two patterns:
Pdef( \tickToWonder ).play;


// using pos arrays directly:
Pdef( \tick, Pbind( \degree, Pxrand([0,3,5,3,8,7,4],inf), \octave, Pseq([7,6,8],inf), \amp, [0.3,0.1,0.2], \dur, 0.05, \delta, Pfunc({0.1.rand + 0.1}), \instrument, \pgrain, \out, Pseq( [s.myOuts[0], s.myOuts[1], s.myOuts[2] ], inf ), \pos, Pseq( [ [-5,3], [5,4], [-8,5], [3,4], [0,8], [-5,2] ] + [[0,-10]], inf ) + Pstutter( 20, Pseq( [ [0,0], [0,8], [0,16], [0,24], [0,32] ].mirror, inf ) ) ) );


Pdef( \tickToWonder ).stop;

