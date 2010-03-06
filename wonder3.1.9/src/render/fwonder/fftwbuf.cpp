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

#include "fftwbuf.h"
#include "complex_mul.h"
#include <iostream>
#include <cerrno>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::endl;

FftwBuf::FftwBuf( size_t bufferSize ) : size( bufferSize )
{
    // set the alignment of allocated memory
    // fftw requires 16-byte alignment in order to support SIMD instructions
    size_t alignment = 16;

    void* mem; 

    //XXX: why is it bufferSize + 8, shouldn't it be 2 the way fftw works? 2*(N/2+1)
    //XXX: maybe use fftw_malloc instead?
    int error = posix_memalign( &mem, alignment, ( bufferSize + 8 ) * sizeof( float ) );
    if( error != 0 )
    {
        cerr << "Error! Unable to allocate aligned memory." << endl;
        if( error == ENOMEM )
        {
            cerr <<"Insufficient memory with the requestet alignment of " << alignment << endl;
        }
        else if( error == EINVAL )
        {
            cerr <<"The requestet alignment of " << alignment 
                 << " is not a power of two or not a multiple of sizeof( void* ) which is " 
                 << sizeof( void* ) << endl;
        }
        exit( EXIT_FAILURE );
    }

    samples = reinterpret_cast< float* >( mem );

    // initialize the plans for the in-place fft and ifft
    //if( ! plansMade )
    {
         fft_plan = fftwf_plan_dft_r2c_1d( bufferSize, samples, reinterpret_cast< fftwf_complex* > ( samples ), FFTW_MEASURE );
        ifft_plan = fftwf_plan_dft_c2r_1d( bufferSize, reinterpret_cast< fftwf_complex* > ( samples ), samples, FFTW_MEASURE );

        if( ! fft_plan  ||  ! ifft_plan )
        {
            cerr << "Error! Could not create fft plan." << endl;
            exit( EXIT_FAILURE );
        }

        plansMade = true;
    }
}


FftwBuf::~FftwBuf()
{
    if( plansMade )
    {
        //XXX: assuming that the plans exist,  so far I found no way to check this
        fftwf_destroy_plan( fft_plan );
        fftwf_destroy_plan( ifft_plan );
        plansMade = false;
    }

    if( samples )
    {
        free( samples );
        samples = NULL;
    }
}


void FftwBuf::fft()
{
    fftwf_execute( fft_plan );
    complex_to_sse_order( samples, getSSESize() );
}

void FftwBuf::ifft()
{
    sse_to_complex_order( samples, getSSESize() );
    fftwf_execute( ifft_plan );
}


void FftwBuf::zeroPad2ndHalf()
{
    for( unsigned int i = size / 2; i < getSSESize(); ++i )
        samples[ i ] = 0.0;
}



float* FftwBuf::getSamples()
{
    return samples;
}


size_t FftwBuf::getRealSize()
{
    return size;
}


size_t FftwBuf::getComplexSize() 
{
    return ( size + 2 );
}


size_t FftwBuf::getSSESize()
{ 
    return ( size + 8 );
}


void FftwBuf::clear()
{
    for( unsigned int i = 0; i < getSSESize(); ++i )
        samples[ i ] = 0.0;
}


void FftwBuf::print()
{
    for( unsigned int i = 0; i < getSSESize(); i += 8 )
    {
        cout << "( ( ";
        for( int j = 0; j < 4; ++j )
            cout << samples[ i + j ] << ", ";

        cout << ")  ( ";

        for( int j = 4; j < 8; ++j )
            cout << samples[ i + j ] << ", ";

        cout << ")" << endl;
    }
}
