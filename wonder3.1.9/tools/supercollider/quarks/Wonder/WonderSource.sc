WonderSource{
	var <id;
	var <oscout;
	var <pos;
	var <type;

	*new { |id,type,w|
		^super.new.init( id, w, type ).myInit;
	}

	init{ |i,w,tp=1|
		oscout = w;
		pos = Point( 0, 0 );
		type = tp;
		this.id = i;
	}

	wonder_{ |w|
		oscout = w;
		oscout.type( id, type );
		this.sendpos;
	}

	id_{ |i|
		id = i;
		oscout.type( id, type );
		this.sendpos;
	}

	pos_{ |newpos,dur=0.2,time=0|
		pos = newpos;
		this.sendpos( dur, time );
	}

	move{ |vector, dur=0.2, time=0|
		pos = pos.translate( vector );
		this.sendpos( dur, time );
	}

	move_polar{ |angle,r, dur=0.2, time=0|
		var vector = Point(0,0);
		vector = vector.translate( Point( r, 0 ) );
		vector = vector.rotate( angle );
		pos = pos.translate( vector );
		this.sendpos( dur, time );
	}

	sendpos { |dur=0.2, time=0|
		oscout.pos( id, pos.x, pos.y, duration: dur, time: time );
	}

}
