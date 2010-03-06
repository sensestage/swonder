

#ifndef PLANEWAVE_H
#define PLANEWAVE_H


#include "vector2d.h"

class PlaneWave {
    private:
	Vector2D pos;
	Vector2D n;

    public:
	PlaneWave( float x=0.0, float y=0.0, float nx=0.0, float ny=0.0 ) {
	    pos[0] = x;
	    pos[1] = y;
	    n[0] = nx;
	    n[1] = ny;
	}

	PlaneWave( Vector2D &position, Vector2D &normal ) {
	    pos = position;
	    n = normal;
	}

	Vector2D &getPos() { return pos; }
	Vector2D &getN() { return n; }
};

#endif // PLANEWAVE_H
