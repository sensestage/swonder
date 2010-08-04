/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *  Torben Hohn, Eddie Mond, Marije Baalman                                          *
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

#ifndef DELAYCOEFF_H
#define DELAYCOEFF_H

#include "twonder_config.h"

#include <cmath>
#include <iostream>

// This is the maximum distance a Source can be in front of a speaker without letting the factor calculation explode. 
// unit is meters
const float deltaR = 30.0;

/**
 * @brief DelayCoeff is an index into a DelayLine. Consists of @a delay and @a factor.
 *
 * A DelayCoeff models a dirac impuls on an ImpulseResponse. The @a delay is the t-Position on the response.
 * And the @a factor is the "height" of the dirac. Hey.. This is digital... So it IS the height ;)
 */
class DelayCoeff 
{
public:
    DelayCoeff( float delay = 0.0, float factor = 0.0 ) : delay( delay ), factor( factor )
    { 
    }

    float getDelay()
    {
        return delay; 
    }

	/**
	 * @brief Get the delay in samples.
	 *
	 * @param negdelay  The maximum negative delay which is handled, before entering the magic non casual space. Where the most evil sounding distortions occur.
	 *                 This must be reality distortion or something ;)
	 * 
	 * Calculate the sample delay at SampleRate 44100 and Velocity of sound 340. Also makes the delay a casual delay by
	 * adding the @a negdelay which is in meters.
	 */

    unsigned int getSampleDelayRounded( float preDelay );
    float        getSampleDelay       ( float preDelay );


    void setDelay( float d )
    {
        delay = d;
    }

    float getFactor()
    { 
        return factor;
    }

    void setFactor( float c )
    {
        factor = c;
    }

    void print();
        
private:
    float delay; // in meters
    float factor;
};

#endif
