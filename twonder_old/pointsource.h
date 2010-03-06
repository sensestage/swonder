
#ifndef POINTSOURCE_H
#define POINTSOURCE_H

#include "vector2d.h"

class PointSource {
    private:
	Vector2D pos;
	Vector2D target_pos;

	Vector2D block_target_pos;
	int	 target_time;

    public:
	PointSource( float x=0.0, float y=0.0 ) {
	    pos[0] = x;
	    pos[1] = y;
	    target_pos[0] = x;
	    target_pos[1] = y;
	}

	PointSource( Vector2D &position ) { pos = position; target_pos = position; }
	
	const Vector2D &getPos() { return pos; }
	const Vector2D &getTargetPos() {
	    if( target_time == 0 ) {
		return target_pos;
	    } else {
		block_target_pos = (target_pos - pos) * (1.0/(float) target_time);
		return block_target_pos;
	    }
		
	}
	void applyTargetPos() {
	    if( target_time == 0 ) {
		pos = target_pos;
	    } else {
		pos = (target_pos - pos) * (1.0/(float) target_time);
		target_time -= 1;
	    }
	}

	void setPos( Vector2D &newpos ) { target_pos = newpos; target_time = 0; }
	void setPos( Vector2D &newpos, int time ) { target_pos = newpos; target_time = time; }

	
};

#endif // POINTSOURCE_H
