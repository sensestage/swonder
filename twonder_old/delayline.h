
#ifndef DELAYLINE_H
#define DELAYLINE_H

#include "delaycoeff.h"

#include <valarray>

/**
 * @brief Class to hold and manipulate blocks of samples according to DelayCoeffs, fading ...  
 */
class DelayLine {
    private:
	float negdelay;

	// XXX WARNING: must be power of two.
	static int len;

	float *line;
	int write_pos;
    public:
	DelayLine( float max_negative_delay = 10.0 );
	~DelayLine();

	void set_negdelay( float new_negdelay );
	// XXX WARNING: must only write power of 2;
	void put( float *samples, unsigned int nsamples );
	void get( DelayCoeff &coeff, float *samples, unsigned int nsamples );
	void get( int sample_delay, float *samples, unsigned int nsamples );
	void get( DelayCoeff &coeff0, DelayCoeff &coeff1, float *samples, unsigned int nsamples );
	void get_interp( DelayCoeff &coeff0, DelayCoeff &coeff1, float *samples, unsigned int nsamples );
	void get_fadej( DelayCoeff &coeff0, DelayCoeff &coeff1, float *samples, unsigned int nsamples );
	void accumulate_at( int pos, float *samples, unsigned int nsamples );
	void accumulate_fadeout_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfadde_samples );
	void accumulate_fadein_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfadde_samples );
	void accumulate_fadeout2_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfadde_samples );
	void accumulate_fadein2_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfadde_samples );
	void get_advance( float *samples, unsigned int nsamples );

};


#endif
