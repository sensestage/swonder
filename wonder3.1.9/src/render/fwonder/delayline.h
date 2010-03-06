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

#ifndef DELAYLINE_H
#define DELAYLINE_H

class DelayLine
{

public:
    DelayLine( float maxDelay = 10.0 );
    ~DelayLine();

    // XXX WARNING: must only write power of 2;
    void put( float* samples, unsigned int nsamples );

    void get( float* samples, unsigned int nsamples, int sample_delay = 0 );

    void accumulateAt        ( int pos, float* samples, unsigned int noSamples );
    void accumulateFadeOutAt ( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples );
    void accumulateFadeInAt  ( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples );
    void accumulateFadeOut2At( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples );
    void accumulateFadeIn2At ( int pos, float* samples, unsigned int noSamples, unsigned int noCrossfadeSamples );

    void getAndClearAndAdvance( float* samples, unsigned int noSamples );

private:
    DelayLine( const DelayLine& other );

    // the length of this buffer in samples
    // XXX WARNING: must be power of two.
    int lineLength;

    float  maxDelay;
    float* line;
    int    writePos;

};

#endif
