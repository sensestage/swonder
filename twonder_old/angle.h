
#ifndef ANGLE_H
#define ANGLE_H

#include <cmath>
#include "vector2d.h"

/**
 * @brief Angle Type and operations on angles.
 *
 */
class Angle {
    private:
	float ang;

    public:
	Angle( float a );
	Angle( const Angle &other );
	const Angle &operator= ( float a ); 
	Angle operator+ (float a) const;
	Angle operator+ (const Angle &other) const;
	Angle operator- (const Angle &other) const;
	Angle operator* (float factor) const;
	operator float () const;
	Vector2D getN() const;
};


#endif
