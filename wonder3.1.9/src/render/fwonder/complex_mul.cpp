/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universität Berlin, Germany                                           *
 *  Audio Communication Group                                                        *
 *  www.ak.tu-berlin.de                                                              *
 *  Copyright 2006-2008                                                              *
 *                                                                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by             *
 *  the Free Software Foundation; either version 2 of the License, or                *
 *  (at your option) any later version.                                              *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef float v4sf __attribute__ ((vector_size (16)));
 
/**
 * Complex Multiplication
 *
 * This adds the results of the complex multiplications of the
 * values in @a in1 and @a in2 into the output array @a out
 *
 * @param n number of floats which constitute the complex arrays.
 *	    this must be a multiple of 8
 * @param in1 ptr to first factor array.
 * @param in2 ptr to second factor array.
 * @param out ptr to result array
 *
 */

void complex_mul( float* in1, float* in2, float* out, unsigned int n )
{
    v4sf* input1 = ( v4sf* ) in1;
    v4sf* input2 = ( v4sf* ) in2;
    v4sf* output = ( v4sf* ) out;

    // ok... the input arrays are in sse order RRRR IIII
    // output will be the same.

    v4sf r1;
    v4sf r2;
    v4sf i1;
    v4sf i2;
    v4sf rout;
    v4sf iout;
    
    for( unsigned int i = 0; i < ( n / 8 ); ++i )
    {
	r1 = *( input1++ );
	i1 = *( input1++ );
	r2 = *( input2++ );
	i2 = *( input2++ );

	rout = *( output );
	iout = *( output + 1 );

	rout += (r1*r2 - i1*i2);
	iout += (r1*i2 + i1*r2);

	*( output++ ) = rout;
	*( output++ ) = iout;
    }
}


void complex_mul_overwrite( float* in1, float* in2, float* out, unsigned int n )
{
    v4sf* input1 = ( v4sf* ) in1;
    v4sf* input2 = ( v4sf* ) in2;
    v4sf* output = ( v4sf* ) out;

    // ok... the input arrays are in sse order RRRR IIII
    // output will be the same.

    v4sf r1;
    v4sf r2;
    v4sf i1;
    v4sf i2;
    
    for( unsigned int i = 0; i < ( n / 8 ); ++i )
    {
	r1 = *( input1++ );
	i1 = *( input1++ );
	r2 = *( input2++ );
	i2 = *( input2++ );

	*( output++ ) = (r1*r2 - i1*i2);
	*( output++ ) = (r1*i2 + i1*r2);
    }
}


/**
 * shuffle an array so that the data can be read elegantly using
 * brutefir style complex_mul
 *
 * n must be a multiple of 8 and counts the number of floats.
 * so the number of complex values is n/2.
 *
 * @note the fftw_r2c interface results in n/2+1 complex values, which
 * is not exactly divisible by 8 :(
 *
 */

void complex_to_sse_order( float* in1, unsigned int n )
{
    v4sf a;
    v4sf b;
    v4sf r_vec;
    v4sf i_vec;

    v4sf* in_v = ( v4sf* ) in1;

    for( unsigned int i = 0; i < ( n / 8 ); ++i )
    {
        a = *in_v;
	b = *( in_v + 1 );

	// shuffle reals into one word. 
	// %10001000
	r_vec = a;
#ifdef NDEBUG
	r_vec = __builtin_ia32_shufps( r_vec, b, 0x88 );
#endif
	
	// shuffle imags into one word. 
	// %11011101
	i_vec = a;
#ifdef NDEBUG
	i_vec = __builtin_ia32_shufps( i_vec, b, 0xdd );
#endif
	
	*(in_v++) = r_vec;
	*(in_v++) = i_vec;
    }
}


void sse_to_complex_order( float* in1, unsigned int n )
{
    v4sf a; 
    v4sf b;
    v4sf  r_vec;
    v4sf  i_vec;

    v4sf *in_v = ( v4sf* ) in1;

    for( unsigned int i = 0; i < ( n / 8 ); ++i )
    {
	r_vec = *in_v;
	i_vec = *( in_v + 1 );

	// shuffle first 4 floats.... 
	// r r r r   i i i i
	//         V
	// r i r i   r i r i
	//
	// %01000100
	//
	// = r r i i   r r i i
	//
	// %11011000
	a = r_vec;
#ifdef NDEBUG
	a = __builtin_ia32_shufps( a, i_vec, 0x44 );
	a = __builtin_ia32_shufps( a, a,     0xd8 );
#endif
	
	// shuffle second 4 floats..... 
	// %11101110
	b = r_vec;
#ifdef NDEBUG
	b = __builtin_ia32_shufps( b, i_vec, 0xee );
	b = __builtin_ia32_shufps( b, b,     0xd8 );
#endif
	
	*( in_v++ ) = a;
	*( in_v++ ) = b;
    }
}


// main for testing
//#include <iostream>
//using namespace std;
//
//int main() {
//    float a[8] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };
//
//    float *aligned;
//    posix_memalign( (void **) &aligned, 16, 8*sizeof(float) );
//
//    for( int i=0; i<8; i++ )
//	aligned[i] = a[i];
//    
//    complex_to_sse_order( aligned, 8 );
//    for( int i=0; i<8; i++ )
//	cout << aligned[i] << endl;
//
//    cout << "shuffled back...." << endl;
//
//    sse_to_complex_order( aligned, 8 );
//
//    for( int i=0; i<8; i++ )
//	cout << aligned[i] << endl;
//}
