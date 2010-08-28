WonderOSC : NetAddr {

	activate{ |srcid, onoff=true|
		if ( onoff ){
			this.sendMsg( "/WONDER/source/activate", srcid.asInteger );
		}{
			this.sendMsg( "/WONDER/source/deactivate", srcid.asInteger );
		}
	}

	deactivate{ |srcid|
		this.activate( srcid, false );
	}

	type { arg srcid, type, time = 0;
		this.sendMsg( "/WONDER/source/type", srcid.asInteger, type.asInteger, time.asFloat );
		// type: 0 = plane, 1 = point
	}
	
	pos { arg srcid, posx, posy, duration=0.1, time = 0.0;
		this.sendMsg( "/WONDER/source/position", srcid.asInteger, 
			posx.asFloat, posy.asFloat, // posz.asFloat, 
			duration.asFloat, time.asFloat 
		);
	}

	angle { arg srcid, angle, duration=0.1, time = 0;
		this.sendMsg( "/WONDER/source/angle", srcid.asInteger, 
			angle.asFloat, duration.asFloat, time.asFloat );
	}
	
	doppler { arg srcid, onoff;
		this.sendMsg( "/WONDER/source/dopplerEffect", srcid.asInteger, onoff.asInteger );
		// onoff: 0 = off, 1 = on
	}

	sourceGroup{ |srcid, grpid|
		this.sendMsg( "/WONDER/source/groupId", srcid.asInteger, grpid.asInteger );
	}

	rotatingDirection{ |srcid, inv=0|
		this.sendMsg( "/WONDER/source/rotationDirection", srcid.asInteger, inv.asInteger );
		// inv = 1, inverted; 0, normal
	}

	scalingDirection{ |srcid, inv=0|
		this.sendMsg( "/WONDER/source/scalingDirection", srcid.asInteger, inv.asInteger );
		// inv = 1, inverted; 0, normal
	}

	groupActivate{ |grpid, onoff=true|
		if ( onoff ){
			this.sendMsg( "/WONDER/group/activate", grpid.asInteger );
		}{
			this.sendMsg( "/WONDER/group/deactivate", grpid.asInteger );
		}
	}

	groupDeactivate{ |srcid|
		this.groupActivate( srcid, false );
	}

	groupPosition{  arg srcid, posx, posy; //, duration=0.1, time = 0.0;
		this.sendMsg( "/WONDER/group/position", srcid.asInteger, 
			posx.asFloat, posy.asFloat
			//	duration.asFloat, time.asFloat // no timing yet?
		);
	}

	createProject{ |name|
		this.sendMsg( "/WONDER/project/createWithoutScore", name.asString ); 
	}

	loadProject{ |name|
		this.sendMsg( "/WONDER/project/load", name.asString ); 
	}
	
	saveProject{ |name|
		if ( name.notNil ){
			this.sendMsg( "/WONDER/project/save", name.asString ); 
		}{
			this.sendMsg( "/WONDER/project/save" ); 
		}
	}
	
	takeSnapshot{ |id,name|
		this.sendMsg( "/WONDER/project/snapshot/take", id.asInteger, name.asString ); 
	}

	recallSnapshot{ |id,name|
		this.sendMsg( "/WONDER/project/snapshot/recall", id.asInteger ); 
	}

	deleteSnapshot{ |id,name|
		this.sendMsg( "/WONDER/project/snapshot/delete", id.asInteger ); 
	}

	renameSnapshot{ |id,name|
		this.sendMsg( "/WONDER/project/snapshot/rename", id.asInteger, name.asString ); 
	}

	copySnapshot{ |from, to |
		this.sendMsg( "/WONDER/project/snapshot/copy", from.asInteger, to.asInteger ); 
	}
		
	/*
	highdamp { arg srcid, hd, duration=0.1, time = 0;
		this.sendMsg( "/WONDER/source/highdamp", srcid.asInteger, hd.asString );
		// hd must be "true" or "false"
		}

	refl { arg srcid, rf, duration=0.1, time = 0;
		this.sendMsg( "/WONDER/source/refl", srcid.asInteger, rf.asString );
		// rf must be "true" or "false"
		}
		
	polar { arg srcid, pl, duration=0.1, time = 0;
		this.sendMsg( "/WONDER/source/polar", srcid.asInteger, pl.asString );
		// pl must be "polar" or "cartesian"
		}
		
	room_old { arg srcid, wall1, wall2, wall3, wall4, absorp1, order, duration=0.1, time = 0;
		this.sendMsg( "/WONDER/source/room", srcid.asInteger, wall1.asFloat, wall2.asFloat, wall3.asFloat, wall4.asFloat, absorp1.asFloat,order.asInteger );
		}
		
	room { arg srcid, wall1, wall2, wall3, wall4, absorp1, absorp2, absorp3, absorp4, order, duration=0.1, time = 0;
		this.sendMsg( "/WONDER/source/room", srcid.asInteger, wall1.asFloat, wall2.asFloat, wall3.asFloat, wall4.asFloat, absorp1.asFloat, absorp2.asFloat,absorp3.asFloat, absorp4.asFloat,order.asInteger );
		}

	*/
	/*
	winit { arg initfile = "";
		if ( initfile == "", 
			{ this.sendMsg( "/WONDER/init" ); },
			{ this.sendMsg( "/WONDER/init", initfile.asString ); } );
		}
		
	start {
		this.sendMsg( "/WONDER/start" );
		}	
	
	stop { 
		this.sendMsg( "/WONDER/stop" );
		}

	screc { arg recfile="";
		if ( recfile == "",
			{this.sendMsg( "/WONDER/score/record"); },
			{this.sendMsg( "/WONDER/score/record", recfile.asString); });
		}
		
	scplay { arg playfile="";
		if ( playfile == "",
			{ this.sendMsg( "/WONDER/score/play" ); },
			{ this.sendMsg( "/WONDER/score/play", playfile.asString ); });
		}
		
	scpause { 
		this.sendMsg( "/WONDER/score/pause" );
		}

	sctime { arg time;
		this.sendMsg( "/WONDER/score/time", time.asFloat );
		}
		
	scstop { 
		this.sendMsg( "/WONDER/score/stop" );
		}
	}
	*/
}