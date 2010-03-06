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

#include "complexdelayline.h"
#include "fftwbuf.h"

#include <iostream>

using std::cerr;
using std::endl;


ComplexDelayLine::ComplexDelayLine( size_t bufferSize, size_t noBuffers ) : bufferSize( bufferSize ), noBuffers( noBuffers ), currentBuffer( 0 )
{
    for( unsigned int i = 0; i < noBuffers; ++i )
        buffers.push_back( new FftwBuf( bufferSize ) );
}


ComplexDelayLine::~ComplexDelayLine()
{
    while( ! buffers.empty() )
    {
        delete buffers.back();
        buffers.pop_back();
    }
}


FftwBuf* ComplexDelayLine::operator[]( int noBuffer )
{
    int bufferIndex = ( noBuffer + currentBuffer ) % noBuffers;

    if( bufferIndex < 0 )
    {
        cerr << "[Fwonder Error]: negative index into ComplexDelayLine, using first buffer instead." << endl;
        bufferIndex = 0;
    }

    return buffers[ bufferIndex ];
}


void ComplexDelayLine::clearCurrentBufferAndAdvance()
{
    buffers[ currentBuffer ]->clear();
    ++currentBuffer;

    // warp aroung if last buffer was already reached
    if( currentBuffer >= ( int ) noBuffers )
        currentBuffer = 0;
}
