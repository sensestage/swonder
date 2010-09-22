/// Creating "reflections" of the sound at other locations:

(
x.free;
// some sound we will create echos from:
x = SynthDef( \src, { Out.ar( s.myOuts[0], Decay.ar(Impulse.ar(1), 0.25, LFCub.ar(1200,0,0.1))  ) }).play;
);

w.sourcePosition( s.myOuts[0], Point( -10, 3 ) );

(
y.free;
// The reflection:
// we add an extra delay, to emulate the delay from wall to imaginary source location.
// we add a lowpass filter to simulate the damping of the wall
// we add a amplitude factor to simulate the attenuation
y = SynthDef( \reflection1, { |out=0,in=0,delay=1.0,maxdelay=10,lpf=800,lag=0.5,amp=0.5|
	Out.ar( out, 
		DelayL.ar( LPF.ar( In.ar( in, 1), lpf, amp ), maxdelay, delay.lag(lag) ) )
}).play( s, [ \in, s.myOuts[0], \out, s.myOuts[1] ], \addToTail );
	// position of the mirror image source of the reflection
w.sourcePosition( s.myOuts[1], Point( 15, 7 ) );
);

y.set( \amp, 0.8)
y.set( \lpf, 1500 )
y.set( \delay, 0.08 )

y.free;

// we can also create multiple reflections, from different imaginary source positions:
(
w.addSource( s.myOuts[2] ); 
w.addSource( s.myOuts[3] ); 
w.addSource( s.myOuts[4] ); 
w.addSource( s.myOuts[5] ); 
w.addSource( s.myOuts[6] ); 

w.sourcePosition( s.myOuts[2], Point( 15, 7 ) );
w.sourcePosition( s.myOuts[3], Point( -15, 7 ) );
w.sourcePosition( s.myOuts[4], Point( 10, -5 ) );
w.sourcePosition( s.myOuts[5], Point( -10, -5 ) );
w.sourcePosition( s.myOuts[6], Point( 0, -10 ) );
);

q.reflections = (2..6).collect{ |i| Synth.new( \reflection1, [\in,s.myOuts[0], \out, s.myOuts[i], \lpf, 1500, \amp, 0.8, \delay, 0.04 ], s, \addToTail ) };


(0.2+[ 0.3, 0.9, 0.4, 0.7, 0.5 ]).do{ |it,i| q.reflections[i].set( \amp, it ) };
(100+[ 1000, 1500, 1100, 1300, 1200 ]).do{ |it,i| q.reflections[i].set( \lpf, it ) };
(2*[ 0.02, 0.01, 0.015, 0.012, 0.014 ]).do{ |it,i| q.reflections[i].set( \delay, it ) };

q.reflections.do{ |it| it.free; };
x.free;




// calculate the delay based on the difference in location:
~calcDelayTime = { |pos1,pos2,speedOfSound=343| pos1.dist(pos2)/speedOfSound };

~calcDelayTime.value( Point(-5,10), Point(4,10) );

~calcDelayTime.value( w.sources[0].position, w.sources[1].position );

// calculate the attenuation based on the difference in location (pow controls the rolloff):
~calcAmpFactor = { |pos1,pos2,pow=1| 1/((pos1.dist(pos2)**pow)) };

~calcAmpFactor.value( Point(-5,10), Point(4,10) );

~calcAmpFactor.value( w.sources[0].position, w.sources[1].position );

