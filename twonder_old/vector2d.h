
#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <valarray>

/**
 * @brief Vector2D to hold 2-dim coordinates and operations to work inside Interpolat template.
 */

class Vector2D {
    private:
	float v0, v1;
    public:
	Vector2D();
	Vector2D( const Vector2D & );
	Vector2D( float x, float y );
	float &operator[] ( int i ) { return  i==0 ? v0 : v1;} 
	float operator[] ( int i ) const { return i==0 ? v0 : v1; }
	float len() const { return std::sqrt( v0 * v0 + v1 * v1 ); }
};

inline 
Vector2D::Vector2D()
{
    v0 = 0.0;
    v1 = 0.0;
}

inline Vector2D::Vector2D( float x, float y ) {
    v0 = x;
    v1 = y;
}

inline Vector2D::Vector2D( const Vector2D &src ) {
    v0 = src.v0;
    v1 = src.v1;
}

inline float operator * ( const Vector2D &a, const Vector2D &b ) {
    return a[0] * b[0] + a[1] * b[1];
}

inline Vector2D operator * ( const float f, const Vector2D &b ) {
    return Vector2D( f*b[0], f*b[1] );
}

inline Vector2D operator * ( const Vector2D &b, const float f ) {
    return Vector2D( f*b[0], f*b[1] );
}

inline Vector2D operator + ( const Vector2D &a, const Vector2D &b ) {
    Vector2D res;
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];

    return res;
}

inline Vector2D operator - ( const Vector2D &a, const Vector2D &b ) {
    Vector2D res;
    res[0] = a[0] - b[0];
    res[1] = a[1] - b[1];

    return res;
}


#endif

