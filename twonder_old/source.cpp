

#include "source.h"
#include "twonder_globalconfig.h"

#include "global_array.h"

Source::~Source() { 
}

PositionSource::PositionSource( const Vector2D &p )
    : pos( p ) {
}


float hanning( float x ) {
	return 0.5 * cosf( M_PI * x );
}


// #define DELTA_Z 15.0
// #define garray->focuslimit 10.0
// #define garray->focusmargin 1.0


Vector2D global_translate( const Vector2D &pos, struct translation &t ) {
    Vector2D op( pos[0], pos[1] );
    Vector2D rotz_offset( t.rotZax, t.rotZay );
    Vector2D trans_offset( t.xoffset, t.yoffset );

    float cosalpha = cosf(t.rotZ*2*M_PI/360);
    float sinalpha = sinf(t.rotZ*2*M_PI/360);

    op = op + trans_offset;
    op = op - rotz_offset;


    op = Vector2D( cosalpha*op[0] + sinalpha*op[1], -sinalpha*op[0] + cosalpha*op[1] ); 

    op = op + rotz_offset;

    return op;
}


bool
PointSource::focus_src( Vector2D &src ) {

    if( conf->iomode == IOM_ALWAYSOUT )
		return false;

    if( conf->iomode == IOM_ALWAYSIN ) 
		return true;

/// new version based on global_array
	if( (src[1] < garray->miny) || (src[0] > garray->maxx) || (src[0] < garray->minx) || (src[1] > garray->maxy) )
	    return false;
	else
	    return true;

/// old version
/*    if( conf->iomode == IOM_H104 ) {
		if( (src[1] < 0.0) || (src[0] > 10.0) || (src[0] < -10.0) || (src[1] > 25.0) )
	    	return false;
		else
	    	return true;
    	}*/
//     else
	return true;
}

DelayCoeff
PointSource::calcDelayCoeff( const Speaker &spk, const Vector2D &src_orig, struct translation &t ) {

	Vector2D src = global_translate( src_orig, t );
	Vector2D diff = spk.getPos() - src;
	float normproject = diff * spk.getN();
	float difflen = diff.len();

	float window = 1.0;
	float cosphi;
	// check whether before or behind speaker
	if( normproject < 0.0 ) {

		difflen = -difflen;

		//if( (src[1] < 0.0) || (src[0] > 10.0) || (src[0] < -10.0) || (src[1] > 25.0) ) {
		if( ! focus_src( src ) ) {
			return DelayCoeff( difflen, 0.0 );
		}

// std::cout << "focused source" << std::endl;

		if( conf->headphonemode ) {
		    Vector3D src3D( src[0], src[1], src[1] < 10.0 ? 2.1 : (2.1+(src[1]-10.0) * (4.0/15.0)) );
		    Vector3D diff3D = spk.get3DPos() - src3D;
		    normproject = - (diff3D * spk.get3DN());
		    difflen = diff3D.len();
		    
		} else {

		    if( difflen < (-garray->focuslimit) ) {
			window = hanning( (difflen + garray->focuslimit) / garray->focusmargin );
		    }
		    if( difflen < (- (garray->focusmargin+garray->focuslimit)) )
			return DelayCoeff( difflen, 0.0 );
		    //		if( (src[1] > 0.0) || (src[0] > 8.0) || (src[0] < -8.0) ) {
		    //			return DelayCoeff( difflen, 0.0 );
		    //		}

			/// slope correction:
		    Vector3D src3D( src[0], src[1], src[1] < garray->elevationy1 ? garray->elevationz1 : (garray->elevationz1 + (src[1]-garray->elevationy1) * (garray->slope) ) );
		    Vector3D diff3D = spk.get3DPos() - src3D;
		    normproject = diff3D * spk.get3DN();
		    difflen = -diff3D.len();
		}
	} 

	if( conf->headphonemode ) {
		cosphi = normproject / difflen;
		if( normproject > 0.0 ) 
		    cosphi = 0.5 +0.5*cosphi;
		else
		    cosphi = -0.5 -0.25 * cosphi;
	}
	else
	    cosphi = normproject / difflen;

	float delay = difflen;


	// XXX: good for 1st test.... adjust to swonder soon.
	if( delay < (-deltaR) )
		delay = -deltaR + 0.1;

	//cout << cosphi << "diff... " << diff[0] << " " << diff[1];
	// XXX: This is for refenceline at position deltaR
	// float factor = sqrt( deltaR / ( delay + deltaR ) ) * cosphi / sqrt( delay );
	// This does not work for negative delay. perhaps its    * cosphi / sqrt( delay + deltaR )

	//float factor = sqrt( deltaR / ( delay + deltaR ) ) * cosphi / sqrt( delay + deltaR );
	
	// XXX: need to find the correct thingy here.
	//      sqrt or no-sqrt or some linear combo...
	//
	//float factor = cosphi / sqrt( delay + deltaR );
	//float factor = cosphi / ( delay + deltaR );

	float factor = 0.0;

	float border = garray->speakerdistance * 1.5;

	if( difflen > border ) {
	    factor = sqrtf( garray->reference / ( normproject + garray->reference ) ) * cosphi / sqrtf( difflen );
	} else if ( difflen < (-1*border) ) {
	    factor = sqrtf( garray->reference / ( garray->reference + normproject ) ) * cosphi / sqrtf( -difflen );
	} else {

	    float behind = sqrtf( garray->reference / ( border + garray->reference )) * 1.0 / sqrtf( border );
	    float focuss = sqrtf( garray->reference / ( garray->reference - border )) * 1.0 / sqrtf( border );

// 	    float behind = sqrtf( DELTA_Z / ( 0.15 + DELTA_Z )) * 1.0 / sqrtf( 0.15 );
// 	    float focuss = sqrtf( DELTA_Z / ( DELTA_Z - 0.15 )) * 1.0 / sqrtf( 0.15 );
// 	    factor = behind + (normproject + 0.15) / 0.3 * ( focuss - behind );

		/// this seems to be correct: normproject goes from +0.15 to -0.15!
	    factor = behind + (border - normproject) / (2*border) * ( focuss - behind );

	}

	//print();
	return DelayCoeff( delay, window * factor * spk.getCosAlpha() );
}

DelayCoeff
PointSource::getDelayCoeff( const Speaker &spk ) {
    return calcDelayCoeff( spk, pos.getVal(), conf->gtrans_old );
}

DelayCoeff
PointSource::getTargetDelayCoeff( const Speaker &spk ) {
    return calcDelayCoeff( spk, pos.getTargetVal(), conf->gtrans );
}

void
PointSource::applyInterpolatTarget() {
    pos.applyTargetVal();
}

PointSource::~PointSource() {
}


/**
DelayCoeff::DelayCoeff( Speaker &spk, PointSource &src ) {

    Vector2D diff = spk.getPos() - src.getPos();
    float cosphi = diff * spk.getN() / diff.len();
    //cout << src.getPos()[0] << "  " << src.getPos()[1] << endl;
    //cout << cosphi << "  " << diff[0] << "  " << diff[1] << endl;

    // XXX: check whether before or behind speaker
    //      1. if( cosphi .... )
    //      2. define other len function
    //         then check whether negative... (???)
    
    delay = diff.len();
    
    
    // XXX: good for 1st test.... adjust to swonder soon.
    if( delay == -deltaR )
	delay += 0.01;

    //cout << cosphi << "diff... " << diff[0] << " " << diff[1];
    // XXX: This is for refenceline at position deltaR
    factor = sqrt( deltaR / ( delay + deltaR ) ) * cosphi / sqrt( delay );

    //print();
}
*/


DelayCoeff
PlaneWave::calcDelayCoeff( const Speaker &spk, const Vector2D &src_orig, const Angle &ang, struct translation &t ) {

    Vector2D src = global_translate( src_orig, t );
    Vector2D diff = spk.getPos() - src;
    float cosphi = spk.getN() * ang.getN();
    float cospsi = (ang.getN() * diff)/diff.len();
    float delay = diff.len() * cospsi;

    // XXX: multiply with volume factor. (is that part of speaker ? probalbly yes)
    // TODO: not implemented yet.
    //       the planewaves are much louder than the point sources.
    //       this is of course obvious. but... how much ?

	/// * cos alpha
    float factor = cosphi;

    if( cosphi < 0.0 ) {
	    return DelayCoeff( delay, 0.0 );
    } else {
	    return DelayCoeff( delay, factor * spk.getCosAlpha() * conf->planecomp );
    }

}

DelayCoeff
PlaneWave::getDelayCoeff( const Speaker &spk ) {
    return calcDelayCoeff( spk, pos.getVal(), angle.getVal(), conf->gtrans_old );
}

DelayCoeff
PlaneWave::getTargetDelayCoeff( const Speaker &spk ) {
    return calcDelayCoeff( spk, pos.getTargetVal(), angle.getTargetVal(), conf->gtrans );
}
void
PlaneWave::applyInterpolatTarget() {
    pos.applyTargetVal();
    angle.applyTargetVal();
}

PlaneWave::~PlaneWave() {
}
