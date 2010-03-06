

#include "delayline.h"

int DelayLine::len = 65536; //< The lenght in Samples of the delayline. Should be power of 2.

/**
 * @brief Create a DelayLine.
 *
 * @param max_negative_delay The maximum possible negative delay this DelayLine can handle. In meters.
 */
DelayLine::DelayLine( float max_negative_delay )
    : write_pos( 0 ), negdelay( max_negative_delay ) {

    line = new float[len];
    // nulling the new delayline
    for( int i=0; i<len; i++ )
	line[i] = 0.0;
}

DelayLine::~DelayLine() {
    delete [] line;
}

void 
DelayLine::set_negdelay( float new_negdelay ) {
    negdelay = new_negdelay;
}
/**
 * @brief Accumulate @a nsamples located at @a samples into the delayline at @a pos
 *
 * @param samples  Pointer to sample buffer. Usually this is the input jackport.
 * @param nsamples Number of samples.
 *
 * This function does not advance the write pointer.
 */

void
DelayLine::accumulate_at( int pos, float *samples, unsigned int nsamples ) {
    // XXX: use memcpy...
    float *accptr = &( line[write_pos + pos] );

    for( int i = 0; i<nsamples; i++ ) {
	if( accptr >= (line+len) )
	    accptr = line;

	*accptr += samples[i];

 	accptr++;
    }
}

void
DelayLine::accumulate_fadeout_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfade_samples ) {
    // XXX: use memcpy...
    float *accptr = &( line[write_pos + pos] );

    float fade=1.0;
    float diff=1.0/(float)crossfade_samples;
    for( int i = 0; i<nsamples; i++ ) {
	if( accptr >= (line+len) )
	    accptr = line;

	*accptr += (samples[i]*fade);
	if( i>crossfade_samples )
	    fade-=diff;
	if( fade <= 0.0 ) {
	    fade=0.0;
	}

 	accptr++;
    }
}

void
DelayLine::accumulate_fadein_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfade_samples ) {
    // XXX: use memcpy...
    float *accptr = &( line[write_pos + pos] );

    float fade=0;
    float diff=1.0/(float)crossfade_samples;
    for( int i = 0; i<nsamples; i++ ) {
	if( accptr >= (line+len) )
	    accptr = line;

	*accptr += (samples[i]*fade);
	if( i>crossfade_samples )
	    fade+=diff;
	if( fade >= 1.0 ) {
	    fade=1.0;
	}

 	accptr++;
    }
}

void
DelayLine::accumulate_fadeout2_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfade_samples ) {
    // XXX: use memcpy...
    float *accptr = &( line[write_pos + pos] );

    float fade=1.0;
    float diff=1.0/(float)crossfade_samples;
    for( int i = 0; i<nsamples; i++ ) {
	if( accptr >= (line+len) )
	    accptr = line;

	*accptr += (samples[i]*fade);
	fade-=diff;
	if( fade <= 0.0 ) {
	    fade=0.0;
	}

 	accptr++;
    }
}

void
DelayLine::accumulate_fadein2_at( int pos, float *samples, unsigned int nsamples, unsigned int crossfade_samples ) {
    // XXX: use memcpy...
    float *accptr = &( line[write_pos + pos] );

    float fade=0;
    float diff=1.0/(float)crossfade_samples;
    for( int i = 0; i<nsamples; i++ ) {
	if( accptr >= (line+len) )
	    accptr = line;

	*accptr += (samples[i]*fade);
	fade+=diff;
	if( fade >= 1.0 ) {
	    fade=1.0;
	}

 	accptr++;
    }
}
void
DelayLine::get_advance( float *samples, unsigned int nsamples ) {

    float *accptr = &( line[write_pos] );
    
    for( int i=0; i<nsamples; i++ ) {
	if( accptr >= (line+len) )
	    accptr = line;

	samples[i] = *accptr;
	*accptr = 0.0;
	accptr++;
    }
    write_pos += nsamples;
    if( write_pos >= len )
	write_pos -= len;
}

/**
 * @brief Put @a nsamples located at @a samples into the delayline.
 *
 * @param samples  Pointer to sample buffer. Usually this is the input jackport.
 * @param nsamples Number of samples.
 *
 * The logic of put and get, assumes, that you @a put() before you @a get().
 * Otherwise the delays wont be correct. So be careful ;)
 */

void
DelayLine::put( float *samples, unsigned int nsamples ) {
    // XXX: use memcpy...
    for( int i = 0; i<nsamples; i++ ) {
	line[write_pos] = samples[i];
	write_pos += 1;
	if( write_pos >= len )
		write_pos = 0;
    }
}

/**
 * @brief Accumulate Samples from Delayline corresponding to @a coeff into @a samples
 *
 * @param coeff   The DelayCoeff you want to get the samples for.
 * @param samples Buffer where to accumulate the samples. Remember to set this to 0.0 before.
 * @param nsamples Size of @a samples.
 */

void
DelayLine::get( DelayCoeff &coeff, float *samples, unsigned int nsamples ) {
    
    // XXX: check write_pos. points after written block now.
    int sdel = write_pos - nsamples - coeff.getSampleDelay( negdelay );
    if( sdel < 0 )
	sdel += len;

    if( (sdel < 0) || (sdel >= len ) )
	    printf( "Error \n" );
    //coeff.print();
    for( int i=0; i<nsamples; i++ ) {
	samples[i] += line[sdel] * coeff.getFactor();
	sdel += 1;
	if( sdel > len )
	    sdel = 0;
    }
}

/**
 * @brief Put Samples from Delayline corresponding to @a delay into @a samples
 *
 * The other Functions of the delayline accumulate into the destination.
 * This function overwrites the contents of the destination.
 *
 * @param sample_delay   The Delay in Samples.
 * @param samples Buffer where to accumulate the samples. Remember to set this to 0.0 before.
 * @param nsamples Size of @a samples.
 */

void
DelayLine::get( int sample_delay, float *samples, unsigned int nsamples ) {
    
    // XXX: check write_pos. points after written block now.
    int sdel = write_pos - nsamples - sample_delay;
    if( sdel < 0 )
	sdel += len;

    if( (sdel < 0) || (sdel >= len ) )
	    printf( "Error \n" );

    for( int i=0; i<nsamples; i++ ) {
	samples[i] = line[sdel];
	sdel += 1;
	if( sdel > len )
	    sdel = 0;
    }
}


/**
 * @brief Accumulate Samples from Delayline corresponding to @a coeff and @a coeff2 into @a samples
 *
 * @param coeff0   The first DelayCoeff you want to get the samples for.
 * @param coeff1   The second DelayCoeff you want to get the samples for.
 * @param samples Buffer where to accumulate the samples. Remember to set this to 0.0 before.
 * @param nsamples Size of @a samples.
 *
 * This Function does a linear interpolation between the Coefficients during the rendering.
 * So this is basically able to implement Movements of sources. 
 *
 * Note that the resampling algorithm implemented here is bad.
 * no samplevalue interpolation.
 * bad performance due to use of lrintf().
 */

void
DelayLine::get( DelayCoeff &coeff0, DelayCoeff &coeff1, float *samples, unsigned int nsamples ) {
    
    // XXX: check write_pos. points after written block now.
    int sdel = write_pos - nsamples - coeff0.getSampleDelay( negdelay );
    if( sdel < 0 )
	sdel += len;

    if( (sdel < 0) || (sdel >= len ) )
	    printf( "Error \n" );
    
    float sdel_f=(float) sdel;
    float dsdel = 1 + ((float) coeff1.getSampleDelay( negdelay ) - coeff0.getSampleDelay( negdelay )) / (float)nsamples;

    float factor = coeff0.getFactor();
    float dfactor = (coeff1.getFactor() - coeff0.getFactor()) / nsamples;
    float len_f = (float) len;

    for( int i=0; i<nsamples; i++ ) {
	samples[i] += line[lrintf(sdel_f)] * factor;
	sdel_f += dsdel; 
	factor += dfactor;
	if( sdel_f > len_f )
	    sdel_f -= len_f;
    }
}

/**
 * @brief Accumulate Samples from Delayline corresponding to @a coeff and @a coeff2 into @a samples
 *
 * @param coeff0   The first DelayCoeff you want to get the samples for.
 * @param coeff1   The second DelayCoeff you want to get the samples for.
 * @param samples Buffer where to accumulate the samples. Remember to set this to 0.0 before.
 * @param nsamples Size of @a samples.
 *
 * This Function does a linear interpolation between the Coefficients during the rendering.
 * So this is basically able to implement Movements of sources. 
 *
 * This does linear resampling. The Array indizes are calculated using a modified version
 * of the Bresenham algorithm. This function is faster than the function above. And the result is also
 * better. Nice ;)
 */

void
DelayLine::get_interp( DelayCoeff &coeff0, DelayCoeff &coeff1, float *samples, unsigned int nsamples ) {

    int signed_nsamples = nsamples;
    

    float factor = coeff0.getFactor();
    float dfactor = (coeff1.getFactor() - coeff0.getFactor()) / signed_nsamples;

    int c0delay = coeff0.getSampleDelay(negdelay);
    int c1delay = coeff1.getSampleDelay(negdelay);

    if( c0delay < 0 ) {
	c0delay = 0;
	//printf( "negative delay after noncausality compenstation !!!\n" );
	factor = 0;
	dfactor = 0;
    }

    if( c1delay < 0 ) {
	c1delay = 0;
	//printf( "negative delay after noncausality compenstation !!!\n" );
	factor = 0;
	dfactor = 0;
    }

    // XXX: dt < 0 !!!!
    //
    int sdel = write_pos - signed_nsamples - c0delay;
    if( sdel < 0 )
	sdel += len;

    if( (sdel < 0) || (sdel >= len ) ) {
	printf( "Error \n" );
    }


    // XXX: c0 - c1 ? 
    int    dt = c0delay - c1delay + signed_nsamples;

    float *readptr = &( line[sdel] );
    float  inter_add = (float) dt / (float) signed_nsamples;

// Until i find out why it the noise is away,
// i leave this at 1.0
//
// noise is almost inaudible at 0.5

// #define INTERPOL_MUST_BE_1 1.0
#define INTERPOL_MUST_BE_1 0.5

    if( dt < 0 ) {
	while( inter_add <= -1.0 ) inter_add += 1.0;

	if( -dt <= signed_nsamples ) {

	    float interpol=INTERPOL_MUST_BE_1;
	    int   acc = signed_nsamples/2;

	    for( int i=0; i<signed_nsamples; i++ ) {

		float out = (*readptr);

		out *= interpol;
		interpol = 1.0-interpol;

		readptr--;     if( readptr < line ) readptr+=len;

		out+=(*readptr)*interpol;
		interpol += inter_add;
		if( interpol < 0.0 ) interpol += 1.0;

		interpol=1.0-interpol;

		acc+=dt; // dt < 0

		if( acc<0 ) {
		    acc += signed_nsamples;
		} else {
		    readptr++; if( readptr > (line + len)) readptr-=len;
		}

		samples[i] += out * factor;
		factor += dfactor;
	    }
	} else {
	    float interpol=INTERPOL_MUST_BE_1;
	    int sadd = dt/signed_nsamples+1;
	    int new_dt = ( -dt ) + (dt/signed_nsamples)*signed_nsamples;
	    int acc = signed_nsamples/2;

	    for( int i=0; i<signed_nsamples; i++ ) {

		float out = (*readptr);

		out *= interpol;
		interpol = 1.0-interpol;

		readptr--;     if( readptr < line ) readptr+=len;

		out+=(*readptr)*interpol;

		interpol += inter_add;
		if( interpol < 0 ) interpol += 1;
		interpol = 1.0-interpol;

		acc -= new_dt; // dt < 0

		if( acc<=0 ) {
		    acc += signed_nsamples; readptr--;
		}

		readptr += sadd;
		if( readptr < line ) readptr+=len;



		samples[i] += out * factor;
		factor += dfactor;
	    }
	}
    } else {
	while( inter_add >= 1.0 ) inter_add -= 1.0;
	if( dt <= signed_nsamples ) {

	    float interpol=INTERPOL_MUST_BE_1;
	    int   acc = signed_nsamples/2;

	    for( int i=0; i<signed_nsamples; i++ ) {

		float out = (*readptr);

		out *= interpol;
		interpol = 1.0-interpol;

		readptr++; if( readptr >= (line + len)) readptr-=len;

		out+=(*readptr)*interpol;
		interpol += inter_add;
		if( interpol >= 1.0 ) interpol -= 1.0;
		interpol = 1.0-interpol;

		acc-=dt; // dt > 0

		if( acc<0 ) {
		    acc += signed_nsamples;
		} else {
		    readptr--;     if( readptr < line ) readptr+=len;
		}

		samples[i] += out * factor;
		factor += dfactor;
	    }
	} else {
	    float interpol=INTERPOL_MUST_BE_1;
	    int sadd = dt/signed_nsamples-1;
	    int new_dt = ( dt ) - (dt/signed_nsamples)*signed_nsamples;
	    int acc = signed_nsamples/2;

	    int debug_j = 0;

//										        printf( "dt = %d, inter_add = %f\n", dt, inter_add );
//										        printf( "sadd = %d, new_dt = %d\n", sadd, new_dt );
	    for( int i=0; i<signed_nsamples; i++ ) {

										    //printf( "====================\n" );
		float out = (*readptr);

		out *= interpol;
		interpol = 1.0-interpol;

										    //printf( "before: %x interpol: %f\n", readptr, interpol );
		readptr++;     if( readptr > (line+len) ) readptr-=len;
										    //printf( "after:  %x\n", readptr );

		out+=(*readptr)*interpol;
		interpol += inter_add;
		if( interpol >= 1.0 ) interpol -= 1.0;
		interpol = 1.0-interpol;

		acc-=new_dt; 

		if( acc<=0 ) {
		    acc += signed_nsamples; readptr++;
		    debug_j ++;
		}

										    //printf( "2before: %x interpol: %f\n", readptr, interpol );
		readptr += sadd;
		if( readptr > (line+len) ) readptr-=len;
										    //printf( "2after:  %x\n", readptr );



		samples[i] += out * factor;
		factor += dfactor;
	    }

//	    printf( "sdel= %d  debug_j=%d\n", sdel, debug_j );
	}
    }
}


/**
 * @brief Accumulate Samples from Delayline corresponding to @a coeff and @a coeff2 into @a samples
 *
 * @param coeff0   The first DelayCoeff you want to get the samples for.
 * @param coeff1   The second DelayCoeff you want to get the samples for.
 * @param samples Buffer where to accumulate the samples. Remember to set this to 0.0 before.
 * @param nsamples Size of @a samples.
 *
 * Linear amplitude interpolation. 
 * Crossfade between the delay positions.
 */

void
DelayLine::get_fadej( DelayCoeff &coeff0, DelayCoeff &coeff1, float *samples, unsigned int nsamples ) {

    int signed_nsamples = nsamples;
    

    float factor = coeff0.getFactor();
    float dfactor = (coeff1.getFactor() - coeff0.getFactor()) / ((float)signed_nsamples);

    int c0delay = coeff0.getSampleDelay(negdelay);
    int c1delay = coeff1.getSampleDelay(negdelay);

    if( c0delay < 0 ) {
	c0delay = 0;
	//printf( "negative delay after noncausality compenstation !!!\n" );
	factor = 0;
	dfactor = 0;
    }

    if( c1delay < 0 ) {
	c1delay = 0;
	//printf( "negative delay after noncausality compenstation !!!\n" );
	factor = 0;
	dfactor = 0;
    }

    // XXX: dt < 0 !!!!
    //
    int sdel0 = write_pos - signed_nsamples - c0delay;
    if( sdel0 < 0 )
	sdel0 += len;

    if( (sdel0 < 0) || (sdel0 >= len ) ) {
	printf( "Error sdel0\n" );
    }

    int sdel1 = write_pos - signed_nsamples - c1delay;
    if( sdel1 < 0 )
	sdel1 += len;

    if( (sdel1 < 0) || (sdel1 >= len ) ) {
	printf( "Error sdel1 \n" );
    }

    float cx_factor = 0;
    float cx_dfactor = 1.0/ ((float) signed_nsamples);

    for( int i=0; i<nsamples; i++ ) {
	samples[i] += line[sdel0] * (1.0-cx_factor) * factor;
	samples[i] += line[sdel1] * (cx_factor) * factor;
	factor += dfactor;
	sdel0 += 1;
	if( sdel0 >= len )
	    sdel0 = 0;
	sdel1 += 1;
	if( sdel1 >= len )
	    sdel1 = 0;

	cx_factor += cx_dfactor;
    }

}


