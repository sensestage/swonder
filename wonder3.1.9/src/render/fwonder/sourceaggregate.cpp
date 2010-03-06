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

#include "sourceaggregate.h"

#include <iostream>
#include <cstdlib>

#include "fftwbuf.h"
#include "fwonder_config.h"
#include "impulseresponse.h"
#include "irmatrixmanager.h"
#include "delayline.h"
#include "jackppclient.h"


jackpp::Client* SourceAggregate::jackClient = NULL;

void SourceAggregate::setJackClient( jackpp::Client* jc )
{
    jackClient = jc;
}


SourceAggregate::SourceAggregate( string jackPortName, int IRPartitionSize, int maxIRLength, int noIRPartitions,
                                  int noTailPartitions, int tailPartitionSize, string IRPath, string tailPath )
{
    inputPort = jackClient->registerPort( jackPortName, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, jackClient->getBufferSize() );

    inputLine = new DelayLine();

    paddingBuffer     = new FftwBuf( IRPartitionSize   * 2 ); 
    tailPaddingBuffer = new FftwBuf( tailPartitionSize * 2 );

    IRManager = new IRMatrixManager( IRPartitionSize, noIRPartitions, IRPath, tailPath );

    ////XXX:
    ////if dynamic grid-change is activated, set static grid resolution ??? 
    //if( fwonderConf->resolutionChangeable )
    //{
    //    pair< int, int > gridResolution     = IRManager->getStaticIRResolutionAt( 0 , 0 );
    //    fwonderConf->staticCacheXResolution = gridResolution.first;
    //    fwonderConf->staticCacheYResolution = gridResolution.second;
    //} 

    // XXX: manager may not return a valid IR if loading of IR failed
    currentIR = IRManager->getBestIR();
    if( ! currentIR )
    {
        std::cerr << "Error! Could not get impulse response from matrix manager. " << std::endl;
        exit( EXIT_FAILURE );
    }
    scheduledIR = currentIR;

    if( fwonderConf->useTail )
        tailIR = IRManager->getTail( tailPartitionSize, noTailPartitions, maxIRLength );
    else 
        tailIR = NULL;

    newIR                = NULL;
    oldIR                = NULL;
    crossfadeInbetweenIR = NULL;
}


SourceAggregate::~SourceAggregate()
{
    if( jackClient )
        jackClient->unregisterPort( inputPort );

    if( inputLine )
    {
        delete inputLine;
        inputLine = NULL;
    }
    if( paddingBuffer )
    {
        delete paddingBuffer;
        paddingBuffer = NULL;
    }
    if( tailPaddingBuffer )
    {
        delete tailPaddingBuffer;
        tailPaddingBuffer = NULL;
    }
    if( IRManager )
    {
        delete IRManager;
        IRManager = NULL;
    }
}
