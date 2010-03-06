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

#ifndef FFTWBUF_H
#define FFTWBUF_H

#include <fftw3.h>

class FftwBuf
{

public:
    FftwBuf( size_t bufferSize );
    ~FftwBuf();

    float* getSamples();

    size_t getRealSize();
    size_t getComplexSize();
    size_t getSSESize();

    void  fft();
    void ifft();

    void zeroPad2ndHalf();

    void clear();
    void print();

protected:
    float* samples;
    size_t size;

    fftwf_plan   fft_plan;
    fftwf_plan  ifft_plan;
    bool        plansMade;

private:
    void /*FftwBuf&*/ operator = ( const FftwBuf& other );
    FftwBuf( const FftwBuf& other );
};

#endif
