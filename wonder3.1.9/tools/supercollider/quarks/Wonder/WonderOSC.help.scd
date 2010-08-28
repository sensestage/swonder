w = WonderOSC.new( "192.168.1.254", 58100 );

// send a position for source 0:
w.pos(0, 5, 5, duration: 10 );


// send an angle for source 0:

w.angle( 0, 90, 0, 10 );

// change the source type:

// type : 1 = point, the third argument is the start angle for the plane wave
w.type( 0, 1, 0, 0 ); w.pos(0, 0, 1, duration: 10 );

w.type( 0, 0, 90, 0 );



// move a source through the array

q = q ? ();

q.pathx = Pseq( Array.series( 50*21, 3, 0.01 ) ++ Array.series( 50*21, 8, -0.01 ), inf ).asStream;
q.pathy = Pseq( Array.series( 20, -20, 1.2 ) ++ Array.series( 20, 3, -1.2 ), inf ).asStream;
q.dur = 0.1;

Tdef( \movesource, { loop{ w.pos( 0, 3, q.pathy.next, duration: q.dur ); q.dur.wait; } } );

Tdef( \movesource ).play;
Tdef( \movesource ).stop;

w.pos( 0, -5, -2, duration: 2 );
w.pos( 0, 5, -2, duration: 2 );
w.pos( 0, -10, -2, duration: 2 );
w.pos( 0, 10, -2, duration: 2 );
w.pos( 0, 0, -10, duration: 2 );

w.pos( 0, 6, 2, duration: 2 );



(Array.series( 50*20, 0, 0.005 ) ++ Array.series( 50*20, 5, -0.005 )).plot
(Array.series( 20, -1, 0.1 ) ++ Array.series( 20, 1, -0.1 )).plot
