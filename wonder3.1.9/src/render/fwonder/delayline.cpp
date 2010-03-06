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

#include "delayline.h"
#include <cstdio>


/**
 * @brief Create a DelayLine.
 *
 * @param maxNegativeDelay The maximum possible negative delay this DelayLine can handle. In meters.
 */

DelayLine::DelayLine( float maxDelay ) : maxDelay( maxDelay ), writePos( 0 )
{
    lineLength= 65536; 

    // create new array of fixed size and initialize ll values with 0.0
    line = new float[ lineLength ];

    for( int i = 0; i < lineLength; ++i )
        line[ i ] = 0.0;
}


DelayLine::~DelayLine()
{
    delete [] line;
}

/**
 * @brief Accumulate @a noSamples located at @a samples into the delayline at @a pos
 *
 * @param samples  Pointer to sample buffer. Usually this is the input jackport.
 * @param noSamples Number of samples.
 *
 * This function does not advance the write pointer.
 */

void DelayLine::accumulateAt( int pos, float* samples, unsigned int noSamples ) 
{
    float* accptr = &( line[ writePos + pos ] );

    for( unsigned int i = 0; i < noSamples; ++i )
    {
	if( accptr >= ( line + lineLength ) )
	    accptr = line;

	*accptr += samples[ i ];
 	++accptr;
    }
}


void DelayLine::accumulateFadeOutAt( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples ) 
{
    float* accptr = &( line[ writePos + pos ] );

    float fadeFactor   = 1.0;
    float fadeStepSize = 1.0 / ( float ) noCrossfadeSamples;

    for( unsigned int i = 0; i < noSamples; ++i )
    {
        if( accptr >= ( line + lineLength ) )
            accptr = line;

	*accptr += ( samples[ i ] * fadeFactor );

	if( i > noCrossfadeSamples )
	    fadeFactor -= fadeStepSize;

	if( fadeFactor <= 0.0 ) 
	    fadeFactor = 0.0;

 	++accptr;
    }
}


void DelayLine::accumulateFadeInAt( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples ) 
{
    float* accptr = &( line[ writePos + pos ] );

    float fadeFactor = 0;
    float fadeStepSize = 1.0 / ( float ) noCrossfadeSamples;

    for( unsigned int i = 0; i < noSamples; ++i )
    {
        if( accptr >= ( line + lineLength ) )
	    accptr = line;

	*accptr += ( samples[ i ] * fadeFactor );

	if( i > noCrossfadeSamples )
	    fadeFactor += fadeStepSize;

	if( fadeFactor >= 1.0 ) 
	    fadeFactor = 1.0;

 	++accptr;
    }
}


void DelayLine::accumulateFadeOut2At( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples ) 
{
    float* accptr = &( line[ writePos + pos ] );

    float fadeFactor = 1.0;
    float fadeStepSize = 1.0 / ( float ) noCrossfadeSamples;

    for( unsigned int i = 0; i < noSamples; ++i )
    {
	if( accptr >= ( line + lineLength ) )
	    accptr = line;

	*accptr    += ( samples[ i ] * fadeFactor );
	fadeFactor -= fadeStepSize;

	if( fadeFactor <= 0.0 ) 
	    fadeFactor = 0.0;

 	++accptr;
    }
}


void DelayLine::accumulateFadeIn2At( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples )
{
    float* accptr = &( line[ writePos + pos ] );

    float fadeFactor = 0;
    float fadeStepSize = 1.0 / ( float ) noCrossfadeSamples;

    for( unsigned int i = 0; i < noSamples; ++i )
    {
	if( accptr >= ( line + lineLength) )
	    accptr = line;

	*accptr    += ( samples[ i ] * fadeFactor );
	fadeFactor += fadeStepSize;

	if( fadeFactor >= 1.0 ) 
            fadeFactor = 1.0;

 	++accptr;
    }
}


void DelayLine::getAndClearAndAdvance( float* samples, unsigned int noSamples ) 
{
    float* accptr = &( line[ writePos ] );
    
    for( unsigned int i = 0; i < noSamples; ++i )
    {
        // wrap around if end of delayline is reached
	if( accptr >= ( line + lineLength ) )
	    accptr = line;

	samples[ i ] = *accptr;
	*accptr      = 0.0;
	++accptr;
    }

    writePos += noSamples;

    if( writePos >= lineLength )
	writePos -= lineLength;
}


/**
 * @brief Put @a noSamples located at @a samples into the delayline.
 *
 * @param samples  Pointer to sample buffer. Usually this is the input jackport.
 * @param noSamples Number of samples.
 *
 * The logic of put and get, assumes, that you @a put() before you @a get().
 * Otherwise the delays wont be correct. So be careful ;)
 */

void DelayLine::put( float* samples, unsigned int noSamples ) 
{
    for( unsigned int i = 0; i < noSamples; ++i )
    {
	line[ writePos ] = samples[ i ];
	++writePos;

        // wrap around if end of buffer is reached
	if( writePos >= lineLength )
            writePos = 0;
    }
}


/**
 * @brief Put Samples from Delayline corresponding to @a delay into @a samples
 *
 * The other Functions of the delayline accumulate into the destination.
 * This function overwrites the contents of the destination.
 *
 * @param samples Buffer where to accumulate the samples. Remember to set this to 0.0 before.
 * @param noSamples Size of @a samples.
 * @param sampleDelay   The Delay in Samples.
 */

void DelayLine::get( float* samples, unsigned int noSamples, int sampleDelay ) 
{
    // calculate starting position 
    int readPos = writePos - noSamples - sampleDelay;

    // wrap around backwards if beginning of buffer is reached
    if( readPos < 0 )
        readPos += lineLength;

    if( ( readPos < 0 ) || ( readPos >= lineLength ) )
    {
        printf( "Error! readPos = %i in DelayLine::get()\n", readPos  );
        // prevent crash, audible clicks are better than complete failure
        readPos = 0;
    }

    // read block of samples
    for( unsigned int i = 0; i < noSamples; ++i )
    {
	samples[ i ] = line[ readPos ];
	++readPos;

        // wrap around if end of buffer is reached
	if( readPos > lineLength )
            readPos = 0;
    }
}
