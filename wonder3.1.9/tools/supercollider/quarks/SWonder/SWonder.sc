
SWonder {
	classvar <>default;
	var <>sources, <cWonder, responders, <connected, <drawGUI;
	
	var <room, <scaledRoom, <>invertX, <>invertY, <wndXMax, <wndXMin, <wndYMax, <wndYMin;
	var <>debug = false;

	var <>gui;

	setDefault{
		default = this;
	}

	*new { |cWonderAddr|

		^super.new.init(cWonderAddr);
		}
	
	init { |cWonderAddr|
		if (cWonderAddr.isNil) {cWonder= NetAddr("127.0.0.1", 58100)} {cWonder = cWonderAddr};
		sources = ();
		this.createResponders;
		this.addResponders; 
		this.room_([ -1 @ 1, 1 @ 1, 1 @ -1, -1 @ -1]);
		connected = false;
		drawGUI = false;
		}
		
	createResponders {
		responders = [
			OSCresponderNode(nil, '/WONDER/stream/render/ping', {|t, r, msg| 
				var id = msg[1];
				if (connected) {cWonder.sendMsg('/WONDER/stream/render/pong', msg[1])}
				}),		
			OSCresponderNode(nil, '/WONDER/stream/visual/ping', {|t, r, msg| 
				var id = msg[1];

				if (connected) {cWonder.sendMsg('/WONDER/stream/visual/pong', msg[1])}
				}),		
			OSCresponderNode(nil, '/WONDER/stream/score/ping', {|t, r, msg| 
				var id = msg[1];
				if (connected) {cWonder.sendMsg('/WONDER/score/render/pong', msg[1])}
				}),		
			OSCresponderNode(nil, '/WONDER/source/activate', {|t, r, msg| 
				var id = msg[1];
				if (sources[id].isNil) {sources[id] = SWonderSrc.new(this).id_(id)
					.name_(id.asString) // dirty - wonder doesn't send names when connecting to stream
					};
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/source/deactivate', {|t, r, msg| 
				var id = msg[1];
				sources[id] = nil;
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/source/position', {|t, r, msg| 
				var id = msg[1];
				if ( gui.notNil ){
					if ( gui.touchedSrc.isNil) {
						sources[id].position_( msg[2] @ msg[3] )
					};
				};
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/source/type', {|t, r, msg| 
				var id = msg[1];
				if (msg[2]==0) {sources[id].plane_(true)}; 
				if (msg[2]==1) {sources[id].plane_(false)};
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/source/angle', {|t, r, msg| 
				var id = msg[1];
				sources[id].angle_(msg[2]);
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/source/dopplerEffect', {|t, r, msg| 
				var id = msg[1];
				if (msg[2]==0) {sources[id].doppler_(false)};
				if (msg[2]==1) {sources[id].doppler_(true)};
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/source/color', {|t, r, msg| 
				var id = msg[1];
				sources[id].color_(Color.new255(msg[2], msg[3], msg[4]));
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/source/name', {|t, r, msg| 
				var id = msg[1];
				sources[id].name_(msg[2]);
				if (debug) {msg.postln};
				}),
			OSCresponderNode(nil, '/WONDER/global/renderPolygon', {|t, r, msg| 
				var newRoom = Array.newClear(msg[2]);
				if (debug) {msg.postln};
				msg[2].do{ |i| newRoom[i] = msg[i*3+3] @ msg[i*3+4]  };
				this.room_(newRoom);
				}),
			OSCresponderNode(nil, '/WONDER/reply', {|t, r, msg| 
				if (debug) {msg.postln};
				if ( msg[2] != 0, { msg.postln; });
				})
			];
		}
		
	addResponders { 
		responders.do{ |resp| resp.add }   
		}

	removeResponders { 
		responders.do{ |resp| resp.remove }   
		}
		
	sendActivate { |id|
		cWonder.sendMsg('/WONDER/source/activate', id);
		if (debug) {["send", '/WONDER/source/activate', id].postln};
		}
	
	sendDeactivate { |id|
		cWonder.sendMsg('/WONDER/source/deactivate', id);
		if (debug) {["send", '/WONDER/source/deactivate', id].postln};
		}
	
	sendPosition { |id, dur= 0|
		var position = sources[id].position;
		//		position = this.norm2wonder(position);
		cWonder.sendMsg('/WONDER/source/position', id, position.x, position.y, dur);
		if (debug) {["send", '/WONDER/source/position', id, position.x, position.y, dur].postln};
		}

	/*
	sendRawPosition { |id, position, dur= 0|
		//	var position = sources[id].position;
		//	position = this.norm2wonder(position);
		sources[id].position = this.wonder2norm( position );
		cWonder.sendMsg('/WONDER/source/position', id, position.x, position.y, dur);
		if (debug) {["send", '/WONDER/source/position', id, position.x, position.y, dur].postln};
		}
	*/

	sendType { |id|
		var type;
		if (sources[id].plane) { type=0 } { type=1 };
		cWonder.sendMsg('/WONDER/source/type', id, type);
		if (debug) {["send", '/WONDER/source/type', id, type].postln};
		}
	
	sendAngle { |id|
		cWonder.sendMsg('/WONDER/source/angle', id, sources[id].angle);
		if (debug) {["send", '/WONDER/source/angle', id, sources[id].angle].postln};
		}
	
	sendDoppler { |id|
		var doppler;
		if (sources[id].doppler) { doppler=1 } { doppler=0 };
		cWonder.sendMsg('/WONDER/source/dopplerEffect', id, doppler);
		if (debug) {["send", '/WONDER/source/dopplerEffect', id, doppler].postln};
		}
		
	sendName { |id|
		cWonder.sendMsg('/WONDER/source/name', id, sources[id].name);
		if (debug) {["send", '/WONDER/source/name', id, sources[id].name].postln};
		}

	sendColor { |id|
		cWonder.sendMsg('/WONDER/source/color', id, 
			sources[id].color.red*255, sources[id].color.green*255, sources[id].color.blue*255);
		if (debug) {["send", '/WONDER/source/color', id, sources[id].color].postln};
		}

	createProject { |name|
		if (connected) { cWonder.sendMsg('/WONDER/project/createWithoutScore', name) }
		}
	
	loadProject { |name|
		if (connected) { cWonder.sendMsg('/WONDER/project/load', name) }
		}
	
	saveProject {
		if (connected) { cWonder.sendMsg('/WONDER/project/save') }
		}
		
	addSource{ |id, position, normalised=false|
		var source = SWonderSrc.new(this);
		position ?? Point(0,0);
		source.id_(id);
		source.name_(id.asString);
		if ( normalised ){
			source.normalisedPosition = position;
		}{
			source.position = position;
		}
		sources[id] = source;
		if (connected) {this.sendActivate(id); this.sendName(id, id.asString); this.sendType(id); this.sendColor(id); this.sendPosition(id); };
		}
		
	addNextSource { |point, norm=false|
		var id;
		if (point.isNil) { point = Point(0,0) };
		10000.do{ |i| if (sources[i] == nil) { this.addSource(i, point, norm); ^i} };
		
		}
		
	removeSource{ |id|
		sources[id] = nil;
		if (connected) { this.sendDeactivate(id) }
		}
		
	sourcePosition { |id, point, dur= 0|
		if (sources[id].notNil) {
		sources[id].position = point;
		if (connected) {this.sendPosition(id, dur)}}	
		}

	sourceNormPosition { |id, point, dur= 0|
		if (sources[id].notNil) {
			sources[id].normalisedPosition = point;
			if (connected) {this.sendPosition(id, dur)}}	
	}

	sourcePlane { |id, bla|
		if (sources[id].notNil) {
		sources[id].plane = bla;
		if (connected) {this.sendType(id) }}	
		}

	sourceAngle { |id, angle|
		if (sources[id].notNil) {
		sources[id].angle = angle;
		if (connected) {this.sendAngle(id)}};
		}
		
	sourceDoppler { |id, doppler|
		if (sources[id].notNil) {
		sources[id].doppler = doppler;
		if (connected) {this.sendDoppler(id)}}
		}
		
	sourceColor { |id, color|
		if (sources[id].notNil) {
		sources[id].color = color;
		if (connected) {this.sendColor(id)}}
		}
	
	sourceName { |id, name|
		if (sources[id].notNil) {
		sources[id].name = name;
		if (connected) {this.sendName(id)}}
		}
		
	room_ { |roomArg, invertXArg=false, invertYArg=true|
		var x = Array.fill(roomArg.size, { |i| roomArg[i].x });
		var y = Array.fill(roomArg.size, { |i| roomArg[i].y });
		var xNorm = x.normalize(-1,1);
		var yNorm = y.normalize(-1,1);
		room = roomArg;
		invertX = invertXArg;
		invertY = invertYArg;
		wndXMax = x.sort.last;
		wndXMin = x.sort.first;
		wndYMax = y.sort.last;
		wndYMin = y.sort.first;
		if (invertX) { xNorm = xNorm * -1 };
		if (invertY) { yNorm = yNorm * -1 };
		scaledRoom = Array.fill(roomArg.size, { |i| xNorm[i] @ yNorm[i] });
		}
		
	drawGUI_{ |bla|
		if (bla) { 
			if (drawGUI.not) 
			{drawGUI= true; this.createGUI} }
			{ if (drawGUI) 
				{drawGUI= false; gui.close; gui = nil }
			}
	}
	
	createGUI {
		gui = SWonderGUI.new( this );
	}

	
	wonder2norm { |pt| 
		var x = pt.x.linlin( wndXMin, wndXMax, -1, 1, nil );
		var y = pt.y.linlin( wndYMin, wndYMax, -1, 1, nil );
		if (invertX) { x = -1 * x };
		if (invertY) { y = -1 * y };		
		^ x @ y;
		}
		
	norm2wonder { |pt|
		var x = pt.x, y = pt.y;
		if (invertX) { x = -1 * x };
		if (invertY) { y = -1 * y };
		x = x.linlin( -1, 1, wndXMin, wndXMax, nil );
		y = y.linlin( -1, 1, wndYMin, wndYMax, nil );
		^ x @ y;
		}
			
	free {
		if (drawGUI) { gui.close };
		this.disconnect;
		responders.do{ |resp| resp.remove; };
		^super.free
		}
		
	connect {
		//		{ cWonder.sendMsg('/WONDER/stream/render/connect', 'SCWonder')}.defer(0.2);
		cWonder.sendMsg('/WONDER/stream/visual/connect', 'SCWonder');
		connected = true;
		}

	disconnect {
		cWonder.sendMsg('/WONDER/stream/render/disconnect', 'SCWonder');
		cWonder.sendMsg('/WONDER/stream/visual/disconnect', 'SCWonder');
		connected = false;
		}
}




SWonderSrc {
	var <>swonder;
	var <>id, <position, <>plane, <>angle, <>doppler, <>color, <>name;

	var <normalisedPosition;
	*new { |swonder,id, position, plane, angle, doppler, color, name|
		^super.newCopyArgs(
			swonder ?? SWonder.default,
			id ?? 0,
			position ?? Point(0,0),
			plane ?? false,
			angle ?? 0,
			doppler ?? false,
			color ?? Color.red,
			name ?? "0"
			);
		}

	position_{ |pos|
		normalisedPosition = swonder.wonder2norm( pos );
	}

	normalisedPosition_{ |npos|
		position = swonder.norm2wonder( npos );
	}
}