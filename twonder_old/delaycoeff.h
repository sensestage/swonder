
#ifndef DELAYCOEFF_H
#define DELAYCOEFF_H

//#include "pointsource.h"
//#include "planewave.h"
#include "speaker.h"
#include "twonder_globalconfig.h"

#include <cmath>
#include <iostream>

const float deltaR = 30.0; //< This is the maximum distance a Source can be before a speaker without letting the factor calculation explode. 

/**
 * @brief DelayCoeff is an index into a DelayLine. Consists of @a delay and @a factor.
 *
 * A DelayCoeff models a dirac impuls on an ImpulseResponse. The @a delay is the t-Position on the response.
 * And the @a factor is the "height" of the dirac. Hey.. This is digital... So it IS the height ;)
 */

class DelayCoeff {
    private:

	float delay;
	float factor;
    public:
	DelayCoeff( float d = 0.0, float f = 0.0 ) : delay(d), factor(f) { }
//	DelayCoeff( Speaker &spk, PointSource &src );
//	DelayCoeff( Speaker &spk, PlaneWave &src );
//	DelayCoeff( Speaker &spk, const Vector2D &src );

	float getDelay() { return delay; }

	/**
	 * @brief Get the delay in samples.
	 *
	 * @param negdelay  The maximus negative delay which is handled, before entering the magic non casual space. Where the most evil sounding distortions occur.
	 *                 This must be reality distortion or something ;)
	 * 
	 * Calculate the sample delay at SampleRate 44100 and Velocity of sound 340. Also makes the delay a casual delay by
	 * adding the @a negdelay which is in meters.
	 */

	int getSampleDelay( float negdelay ) { return lrintf( (delay + negdelay) * (44100/conf->sound_speed) ); }
	float getFactor() { return factor; }

	void setDelay( float d ) { delay = d; }
	void setFactor( float c ) { factor = c; }

	void print() { std::cout << "delay: " << getDelay() << "m SDelay = " << getSampleDelay( 0.0 ) << " Coeff: " << getFactor() << std::endl; }
};

#endif
