// simple tutorial on all the methods:

// Setting up a connection with the WONDER software by using the SWonder class:
w = SWonder( NetAddr("192.168.3.254", 58100)); // creating an instance of SWonder
w.connect; // connect it to cwonder
w.drawGUI_(true); // draw a nice GUI

// Project management
// WONDER needs to have a project loaded, so we create one here.
// Since we are all talking to the same WONDER, only one of us needs to do this for this workshop,
// if you are on your own, you always need to create a project, or load a previous one
w.createProject( "SC-workshop" ); // create a new project

w.loadProject( "SC-workshop" ); // load project if it already existed

w.saveProject( "SC-workshop" ); // save project under a specific name


/*
The GUI allows you to move sources with drag and drop.
Right klicking a plane wave source allows you to turn it.
Pressing 'a' adds a source.
When a source is touched 't' changes it's type, 'a' changes it's angle and 'r'
removes it.
*/

// From code we have the following options:

// adding a new source with a specific ID
w.addSource( s.myOuts[0] ); // no given position, source will be placed at 0,0

w.addSource( s.myOuts[1], 7@0 ); // given position, source will be placed at 7,0

w.addSource( s.myOuts[2], 1@0, true ); // given position, source will be placed at 1,0 in the normalised coordinate system

// changing the position
w.sourcePosition(s.myOuts[2], 3@0, 5);
// first argument is the source ID (== channel)
// second the new position
// third, the duration over which the change should take place

// changing the position, using normalised coordinates:
w.sourceNormPosition(s.myOuts[2], -1.5@0, 5);
// first argument is the source ID (== channel)
// second the new position
// third, the duration over which the change should take place

// make a source a plane wave source
w.sourcePosition(s.myOuts[1], 10@(-10), 5);
w.sourcePlane(s.myOuts[1], true);
w.sourceAngle(s.myOuts[1], 135) // set it's angle

// make it a point source again
w.sourcePlane( s.myOuts[1], false );

// allow for doppler effect, or not
w.sourceDoppler( s.myOuts[1], true );
w.sourceDoppler( s.myOuts[1], false );

// representation
w.sourceName(s.myOuts[1], "new name") // rename it
w.sourceColor(s.myOuts[1], Color.rand) // different color

// Getting a sense of the dimension of H0104

w.sourcePosition( s.myOuts[0], 0@(-2)); // center front
w.sourcePosition( s.myOuts[0], 7@(-2)); // right front
w.sourcePosition( s.myOuts[0], -7@(-2)); // left front
w.sourcePosition( s.myOuts[0], 7@28); // right back
w.sourcePosition( s.myOuts[0], -7@28); // left back


// for the workshop it would be useful if we all used different colors for displaying the sources, so we know which ones are ours in the display, and which one are other people's sources

(
q.clientColors = ();
[ Color.green(0.7), Color.red, Color.blue, Color.cyan, Color.yellow, Color.magenta, Color.blue(0.5), Color.magenta(0.5) ].do{ |it,i|
	q.clientColors.put( i+1, it );
}
);

// this only works on the activated sources!
s.myOuts.do{ |it| w.sourceColor( it, q.clientColors.at( q.myClientID ) ); };

// when adding sources we can do:
w.addSource( s.myOuts[8], color: q.clientColors.at( q.myClientID ) );
