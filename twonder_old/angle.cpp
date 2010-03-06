
#include "angle.h"
#include <cmath>
#include <iostream>

Angle::Angle( float a ) {
    ang = a<0 ? 2*M_PI + fmodf( a,2*M_PI ) : fmodf( a,2*M_PI ) ;
    if( ang >= M_PI )
	ang -= (2*M_PI);
}

Angle::Angle( const Angle &other ) {
    ang = (other.ang<0) ? (2*M_PI + fmodf( other.ang,2*M_PI ) ) : ( fmodf( other.ang,2*M_PI ) );
    //ang = a<0 ? 2*M_PI - fmodf( a,2*M_PI ) : fmodf( a,2*M_PI ) ;
//    if( ang >= M_PI )
//	ang -= (2*M_PI);
}

//Angle::Angle( Angle other ) {
//    ang = other.ang<0 ? M_PI - fmodf( other.ang,2*M_PI ) : fmodf( other.ang,2*M_PI ) ;
//    if( ang > M_PI )
//	ang -= (2*M_PI);
//}

const Angle &
Angle::operator= ( float a ) {
    ang = (a<0) ? (2*M_PI + fmodf( a,2*M_PI )) : (fmodf( a,2*M_PI )) ;
//    if( ang >= M_PI )
//	ang -= (2*M_PI);


    return *this;
}

Angle
Angle::operator-( const Angle &other ) const {
    return Angle( ang - other.ang );
}

Angle
Angle::operator+ ( float a ) const {
    return Angle( ang + a );
}

Angle
Angle::operator+ (const Angle &other) const {
    //std::cout << "ang: op+ " << std::endl;
    return Angle( ang + other.ang );
}

Angle
Angle::operator* (float factor) const {
    return Angle( ang * factor );
}


Angle::operator float () const {
    return ang;
}


Vector2D Angle::getN() const {
    return Vector2D( cosf( ang ), sinf( ang ) );
}
    

