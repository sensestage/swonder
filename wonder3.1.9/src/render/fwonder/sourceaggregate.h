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

#ifndef SOURCEAGGREGATE_H
#define SOURCEAGGREGATE_H

#include <string>

namespace jackpp
{
    class Client;
    class Port;
}

class DelayLine;
class IRMatrixManager;
class ImpulseResponse;
class FftwBuf;


class SourceAggregate
{

public:
    static void setJackClient( jackpp::Client* jc );

    SourceAggregate( std::string jackPortName, int IRPartitionSize, int maxIRLength, int noIRPartitions, int noTailPartitions, 
                     int tailPartitionSize, std::string IRPath, std::string tailPath );
    ~SourceAggregate();

    // members for the RT thread. (taboo)
    jackpp::Port* inputPort;
    DelayLine*    inputLine;

    FftwBuf* paddingBuffer;
    FftwBuf* tailPaddingBuffer;

    ImpulseResponse* currentIR;
    ImpulseResponse* newIR;
    ImpulseResponse* oldIR;
    ImpulseResponse* crossfadeInbetweenIR;
    ImpulseResponse* scheduledIR;

    ImpulseResponse* tailIR;

    // TODO:
    // there needs to be an array of ImpulseResponses.
    // for every partition. Crossfades need to pass through
    // all partitions. 
    // switching all partitions at once produces artifacts.
    //
    // perhaps the CrossfadeIR code is also valid. the artifacts
    // seem to be quite similar.

    // members for the other threads...
    // should be thread safe though. but with normal mutexes...
    IRMatrixManager* IRManager;

private:
    SourceAggregate( const SourceAggregate& other );
    static jackpp::Client* jackClient;

};

#endif
