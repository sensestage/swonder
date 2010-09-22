// Created by Florian Goltz and Marije Baalman

SWonderGUI {

	var <>swonder;

	var <window, winWidth = 460, winHeigth = 460, touchDist = 15, <>touchedSrc, turnSrc;
	var <>maxOut = 3;
	var <>angleOffset=180;


	*new{ |swonder|
		^super.new.swonder_(swonder).init;
	}

	init{ 
		this.createGUI;
	}

	createGUI {
		var view;
		window = Window(\wonder, Rect(100, 100, winWidth, winHeigth))
			.front
			.onClose_({swonder.drawGUI_(false)});
		view = UserView(window, Rect(0, 0, winWidth, winHeigth))
			.background_(Color.new255(247, 247, 247))
			.resize_(5)
			.drawFunc_({
				winWidth = window.bounds.width;
				winHeigth = window.bounds.height;
				Pen.color_(Color.new255(174, 123, 229, 70));
				Pen.addRect(Rect( 0, winHeigth/(maxOut*2)*(maxOut-1), winWidth, winHeigth/maxOut));
				Pen.addRect(Rect(winWidth/(maxOut*2)*(maxOut-1), 0, winWidth/maxOut, winHeigth));
				Pen.draw;
				Pen.strokeColor_(Color.grey);
				Pen.fillColor_(Color.new255(154, 255, 154, 110));
				Pen.moveTo(this.norm2gui( swonder.scaledRoom.last) );
				swonder.scaledRoom.do{ |pt| Pen.lineTo(this.norm2gui(pt)) };
				Pen.fillStroke;
				swonder.sources.do{ |src|
					var pt = this.norm2gui(src.normalisedPosition);
					try {
						Pen.color_(src.color);
						if (src.plane) {
							Pen.addWedge(pt, 12, (src.angle+angleOffset-22.5).degrad, pi/4);	
						} {	
							Pen.addArc(pt, 4, 0, 2*pi);
						};
						Pen.draw;
					};
					Pen.color_(Color.black);
					Pen.stringAtPoint(src.name.asString, pt.x+3 @ pt.y);
					Pen.draw;	
				};
				swonder.groups.do{ |grp|
					var pt = this.norm2gui(grp.normalisedPosition);
					try {
						Pen.color_(grp.color);	
						Pen.addRect( Rect( pt.x - 3, pt.y -3, 6, 6 ) );
						Pen.draw;
						grp.sources.do{ |src|
							var pt2 = this.norm2gui(src.normalisedPosition);
							Pen.line( pt, pt2 );
							Pen.draw;
						};
						Pen.color_(Color.black);
						Pen.stringAtPoint(grp.name.asString, pt.x+3 @ pt.y);
						Pen.draw;
					};
				};
			})
			.mouseDownAction_({|me, x, y, mod, button|
				var closestSrc = this.closestSrc( this.gui2norm( x@y ) );
				if ( closestSrc.isNil.not ) {
					if (this.norm2gui(closestSrc.normalisedPosition).dist( x@y) <= touchDist ) {
						touchedSrc = closestSrc;
						}
					}
				})
			.mouseUpAction_({|me, x, y, mod|
				touchedSrc = nil;
				turnSrc = nil;
				})
			.mouseMoveAction_({|me, x, y, mod|
				if (turnSrc.notNil)
					{ swonder.sourceAngle(turnSrc.id, turnSrc.normalisedPosition.y - y)  } {
					if (touchedSrc.isNil.not)
						{ swonder.sourceNormPosition(touchedSrc.id, this.gui2norm( x@y ) ) }
					};
				})
			.keyDownAction_({|a, b, c, d, e |
				if (touchedSrc.isNil.not) {
					switch ( d )
						{ 114 } { swonder.removeSource(touchedSrc.id); touchedSrc = nil }
						{ 116 } { swonder.sourcePlane( touchedSrc.id, touchedSrc.plane.not ) }
						{  97 } { turnSrc = touchedSrc; }
						} {
					switch ( d )
						{  97 } { swonder.addNextSource  }
						}
				})
			.keyUpAction_({|a, b, c, d, e |
				switch ( d )
					{  97 } { turnSrc = nil }
				});
			if (GUI.current.id == \cocoa ) { view.animate_(true) }
				{ fork { while({ window.isClosed.not }, { window.refresh; (1/5).wait })}; }
		}

	close{
		if ( window.isClosed.not ){
			window.close;
		};
	}

	norm2gui {|pt|
		^pt.x.linlin(-1 , 1, winWidth/(maxOut*2)*(maxOut-1), winWidth/(maxOut*2)*(maxOut+1), nil) @
			pt.y.linlin(-1 , 1, winHeigth/(maxOut*2)*(maxOut+1), winHeigth/(maxOut*2)*(maxOut-1), nil) 
	}
		
	gui2norm { |pt|
		^pt.x.linlin(winWidth/(maxOut*2)*(maxOut-1), winWidth/(maxOut*2)*(maxOut+1), -1, 1, nil) @
			pt.y.linlin(winHeigth/(maxOut*2)*(maxOut-1), winHeigth/(maxOut*2)*(maxOut+1), 1, -1, nil) 
	}

	closestSrc { |pt|
		var closestSrc, dist = inf;
		swonder.sources.do{ |src|
			if (abs(src.normalisedPosition.dist(pt)) < dist) {
					dist = abs(src.normalisedPosition.dist(pt));
					closestSrc = src;
				}
			};
		^closestSrc
		}

}