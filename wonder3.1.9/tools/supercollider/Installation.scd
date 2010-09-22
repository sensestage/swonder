( // defining the quarks path:
~wonder = Quarks(
	reposPath: "",
	// adapt this local path to your path:
	localPath: Document.current.dir +/+ "quarks"
);
);
// install from GUI:
~wonder.gui;

// install from code:
~wonder.install( "SWonder" )



// From the main quarks repository (svn revision 1477 or higher):
Quarks.install( "Republic" );