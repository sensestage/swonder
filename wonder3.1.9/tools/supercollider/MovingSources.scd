// randomly move everything to some place:
(
w.sources.do { |src, i| 
	w.sourceNormPosition( src.id, 2.0.rand2 @ 2.0.rand2, exprand(0.3, 3))
};
);

TdefAllGui.new;


(
x.free;
// some sound we will create echos from:
x = SynthDef( \src, { Out.ar( s.myOuts[2], Decay.ar(Impulse.ar(1), 0.25, LFCub.ar(1200,0,0.1))  ) }).play;
)

// jumping to a new position produces bad glitches
(
Tdef(\randPan, {
	inf.do {
		var dur = exprand(1.0, 10.0);
		var x = 30.0.bilinrand;
		var y = 50.0.bilinrand;
		w.sourcePosition( s.myOuts[2], x@y);
		dur.wait;
	}
}).play;
);

// can also smoothly move there
(
Tdef(\randPan1, {
	inf.do {
		var dur = exprand(1.0, 10.0);
		var x = 30.0.bilinrand;
		var y = 50.0.bilinrand;
		w.sourcePosition( s.myOuts[2], x@y, dur);
		dur.wait;
	}
}).play;
);

Tdef(\randPan).stop;
Tdef(\randPan1).stop;

// make a linear movement from one position to another over a longer time scale
(
Tdef( \line,{
	var totalduration = 20;
	var pos1 = [20,-6];
	var pos2 = [20,25];
	var curpos = pos1;
	var posstep = pos2 - pos1 / totalduration;
	w.sourcePosition( s.myOuts[2], Point(*curpos), 1);
	totalduration.do {
		1.0.wait;
		curpos = curpos + posstep;
		w.sourcePosition( s.myOuts[2], Point(*curpos), 1);
	};
	1.0.wait;
	w.sourcePosition( s.myOuts[2], Point(*pos2), 1);
}).play;
)

( // make an abstracted version, which we can reuse by passing in different start and endpoints, a channel and a duration.
Tdef( \lineStep,{ |envir|
	var curpos = envir.pos1;
	var posstep = envir.pos2 - envir.pos1 / (envir.duration-1);
	w.sourcePosition( envir.chan, Point(*curpos), 1);
	(envir.duration-1).do {
		1.0.wait;
		curpos = curpos + posstep;
		w.sourcePosition( envir.chan, Point(*curpos), 1);
	};
	1.0.wait;
	w.sourcePosition( envir.chan, Point(*envir.pos2), 1);
});
)

( // now we make a rectangular movement around the hall
Tdef( \square, {
	[ [-20,-5], [20,-5], [20, 30], [-20,30], [-20,-5] ].slide(2).clump(2).do{
		|it| Tdef( \lineStep).set( \pos1, it[0], \pos2, it[1], \chan, s.myOuts[2], \duration, 10 ).play;
		10.wait;
	};
});
);

Tdef( \square ).play;


x.free; x = { SinOsc.ar(100) * 0.1 }.play(s, s.myOuts[2]);

// xy pendulum - lissajou figures
(
Tdef(\pendulum, {
	inf.do { |i|
		var xpos = (i * 0.02).sin * 7;
		var ypos = (i * 0.022).sin * 12 + 12;
		w.sourcePosition( s.myOuts[2], xpos@ypos, 0.02 );
		0.02.wait;
	};
}).play;
);

(
Tdef(\pendulum2, {
	inf.do { |i|
		var xpos = (i * 0.024).sin * 7;
		var ypos = (i * 0.028).sin * 12 + 12;
		w.sourcePosition( s.myOuts[2], xpos@ypos, 0.02);
		0.02.wait;
	};
}).play;
);

(
Tdef(\pendulum3, {
	inf.do { |i|
		var xpos = (i * 0.027).sin * 7;
		var ypos = (i * 0.029).sin * 12 + 12;
		w.sourcePosition( s.myOuts[2], xpos@ypos, 0.02 );
		0.02.wait;
	};
}).play;
);

Tdef.all.do{ |it| it.stop; };

x.free;