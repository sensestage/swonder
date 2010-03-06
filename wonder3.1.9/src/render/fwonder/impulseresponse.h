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

#ifndef IMPULSERESPONSE_H
#define IMPULSERESPONSE_H

#include <string>
#include <vector>

#include <sndfile.hh>

#include "fftwbuf.h"

class ImpulseResponseChannel : public std::vector < FftwBuf* >
{

public:
    ImpulseResponseChannel( size_t IRPartitionSize );
    ~ImpulseResponseChannel();

    FftwBuf& addBuffer();

private:
    void /*ImpulseResponseChannel&*/ operator = ( const ImpulseResponseChannel& other );
    ImpulseResponseChannel( const ImpulseResponseChannel& other );
    size_t IRPartitionSize;

};



class ImpulseResponse
{
public:
    enum IRType
    {
        STATIC_IR,
        DYNAMIC_IR,
        TAIL_IR
    };

    ImpulseResponse( std::string fileName, size_t IRPartitionSize, size_t maxNoPartitions, int fadeInPartition, int fadeInLength, IRType type );
    ~ImpulseResponse();

    int    getNoChannels();
    int    getNoPartitions();
    IRType getType();

    int    getFirstPartition() ;
    size_t getPartitionsize();

    ImpulseResponseChannel& getChannel( int i );

    bool   killed;
    int    xPos;
    int    yPos;

private:
    void /*ImpulseResponse&*/ operator = ( const ImpulseResponse& other );
    ImpulseResponse( const ImpulseResponse& other );

    std::vector< ImpulseResponseChannel* > channels;

    size_t IRPartitionSize;
    int    noChannels;
    int    noPartitions;
    int    firstPart;
    IRType type;
};

#endif
