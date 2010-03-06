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

#include "outputaggregate.h"
#include "complexdelayline.h"
#include "delayline.h"

jackpp::Client* OutputAggregate::jackClient = NULL;

OutputAggregate::OutputAggregate( string jackPortName, int IRPartitionSize, int noIRPartitions, int tailPartitionSize, int noTailPartitions )
{
    outputPort = jackClient->registerPort( jackPortName, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, jackClient->getBufferSize() );

    outputLine = new DelayLine();

    complexLine           = new ComplexDelayLine( IRPartitionSize * 2, noIRPartitions );
    
    fadeInComplexLine     = new ComplexDelayLine( IRPartitionSize * 2, noIRPartitions );
    fadeOutComplexLine    = new ComplexDelayLine( IRPartitionSize * 2, noIRPartitions );

    fadeIn2ComplexLine    = new ComplexDelayLine( IRPartitionSize * 2, noIRPartitions );
    fadeOut2ComplexLine   = new ComplexDelayLine( IRPartitionSize * 2, noIRPartitions );

    tailComplexLine       = new ComplexDelayLine( tailPartitionSize * 2, noTailPartitions );
    tailFadeInComplexLine = new ComplexDelayLine( tailPartitionSize * 2, noTailPartitions );
}


OutputAggregate::~OutputAggregate()
{
    if( jackClient )
        jackClient->unregisterPort( outputPort );

    if( complexLine )
    {
        delete complexLine;
        complexLine = NULL;
    }
    if( fadeInComplexLine )
    {
        delete fadeInComplexLine;
        fadeInComplexLine = NULL;
    }
    if( fadeOutComplexLine )
    {
        delete fadeOutComplexLine;
        fadeOutComplexLine = NULL;
    }
    if( fadeIn2ComplexLine )
    {
        delete fadeIn2ComplexLine;
        fadeIn2ComplexLine = NULL;
    }
    if( fadeOut2ComplexLine )
    {
        delete fadeOut2ComplexLine;
        fadeOut2ComplexLine = NULL;
    }
    if( tailComplexLine )
    {
        delete tailComplexLine;
        tailComplexLine = NULL;
    }
    if( tailFadeInComplexLine )
    {
        delete tailFadeInComplexLine;
        tailFadeInComplexLine = NULL;
    }
}


void OutputAggregate::setJackClient( jackpp::Client* jc )
{ 
    jackClient = jc;  
}
