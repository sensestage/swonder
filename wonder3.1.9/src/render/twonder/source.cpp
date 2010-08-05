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

#include "source.h"
#include "delaycoeff.h"
#include "speaker.h"
#include "twonder_config.h"

Source::~Source()
{ }


PositionSource::PositionSource( const Vector2D& p ) : position( p )
{ }

// ------- point source -------

bool PointSource::isFocused( const Vector2D& sourcePos ) const
{
    if( twonderConf->ioMode == IOM_ALWAYSOUT )
        return false;

    if( twonderConf->ioMode == IOM_ALWAYSIN ) 
        return true;

    int noPoints = twonderConf->renderPolygon.size();
    float xSrc = sourcePos[ 0 ]; 
    float ySrc = sourcePos[ 1 ];
    float xnew;
    float ynew;
    float xold;
    float yold;
    float x1;
    float y1;
    float x2;
    float y2;
    bool inside = false;

    xold = twonderConf->renderPolygon[ noPoints - 1 ][ 0 ];
    yold = twonderConf->renderPolygon[ noPoints - 1 ][ 1 ];

    for( int i = 0 ; i < noPoints ; ++i )
    {
          xnew = twonderConf->renderPolygon[ i ][ 0 ];
          ynew = twonderConf->renderPolygon[ i ][ 1 ];
          
          if( xnew > xold ) 
          {
               x1 = xold;
               y1 = yold;
               x2 = xnew;
               y2 = ynew;
          }
          else 
          {
               x1 = xnew;
               y1 = ynew;
               x2 = xold;
               y2 = yold;
          }

          if( ( xSrc > xnew ) == (xSrc <= xold ) && ( ( ySrc - y1 ) * ( x2 - x1 ) < ( y2 - y1 ) * ( xSrc - x1 ) ) )
          {
               inside = ! inside;
          }

          xold = xnew;
          yold = ynew;
     }


    return inside;
}


float hanning( float x ) {
	return cosf( M_PI * x );
}

///NOTE: this "cleaned up"-version was scrubbed of some essential parts
/// - slope correction
/// - transition margin for focused sources (focus range -> focus limit)
/// - global translate (necessary?)
/// 
DelayCoeff PointSource::calcDelayCoeff( const Speaker& speaker, const Vector2D& sourcePos )
{
    Vector2D srcToSpkVec   = speaker.getPos() - sourcePos; 
    float normalProjection = srcToSpkVec * speaker.getNormal(); 
    float srcToSpkDistance = srcToSpkVec.length(); 
    float delay            = srcToSpkDistance;
    float cosphi           = normalProjection / srcToSpkDistance;
    float window = 1.0; // used for interpolation out off focuslimit
    float inFocus; // variable to calculate whether within the focus margin

    // define a circular area around the speakers in which we adjust the amplitude factor to get a smooth
    // transition when moving through the speakers ( e.g. from focussed to non-focussed sources )
    float transitionRadius = twonderConf->speakerDistance * 1.5; // XXX: empirical value

    // if source is in front of speaker
    if( normalProjection < 0.0 ) 
    {
        // don't render this source if it is outside of the defined maximum range
        // for focussed sources
        if( srcToSpkDistance > twonderConf->focusLimit )
            return DelayCoeff( 0.0, 0.0 );
	inFocus = twonderConf->focusLimit - srcToSpkDistance;
	if ( inFocus < twonderConf->focusMargin ){ // fade out within (fadelimit - fademargin up to fadelimit
	    window = hanning( inFocus / twonderConf->focusMargin );
	}

        // don't render this source if it in front of a this speaker 
        // but is not a focussed source
        if( ( ! isFocused( sourcePos ) ) && ( srcToSpkDistance > transitionRadius ) )
            return DelayCoeff( 0.0, 0.0 );
	if ( twonderConf->hasSlope ){
	  // we need a slope correction in case the speaker array has a slope
	  Vector3D src3D( sourcePos[0], sourcePos[1], 
			    sourcePos[1] < twonderConf->elevationY1 ? 
			      twonderConf->elevationZ1 : 
			      (twonderConf->elevationZ1 + (sourcePos[1]-twonderConf->elevationY1) * (twonderConf->slope) ) 
	  );
	  Vector3D diff3D = speaker.get3DPos() - src3D;
	  normalProjection = diff3D * speaker.get3DNormal();
	  srcToSpkDistance = -diff3D.length();
	}
        // if rendering focussed sources we need to use a "negative delay",
        // i.e. make use of a certain amount of already added pre delay
        // and so we don't get any phase inversion we only use positve numbers
        // for our calculations
        delay            = - delay;
	//NOTE: I'm not sure if these should be negative... (MB)
        cosphi           = - cosphi;
     //   normalProjection = - normalProjection;
    } 

    float amplitudeFactor = 0.0;

    // calculate amplitudefactor according to being in- or outside the transition area around the speakers
    if( srcToSpkDistance > transitionRadius )
    {
        amplitudeFactor = ( sqrtf( twonderConf->reference / ( twonderConf->reference + normalProjection ) ) ) * ( cosphi / sqrtf( srcToSpkDistance ) );
    }
    else
    {

        float behind = sqrtf( twonderConf->reference / ( twonderConf->reference + transitionRadius ) ) / sqrtf( transitionRadius );
        float focuss = sqrtf( twonderConf->reference / ( twonderConf->reference - transitionRadius ) ) / sqrtf( transitionRadius );

        amplitudeFactor = behind + ( transitionRadius - normalProjection ) / ( 2 * transitionRadius) * ( focuss - behind );
    }

    return DelayCoeff( delay, amplitudeFactor * speaker.getCosAlpha() * window );
}


DelayCoeff PointSource::getDelayCoeff( const Speaker& speaker )
{
    return calcDelayCoeff( speaker, position.getCurrentValue() );
}


DelayCoeff PointSource::getTargetDelayCoeff( const Speaker& speaker, wonder_frames_t blocksize )
{
    return calcDelayCoeff( speaker, position.getTargetValue( blocksize ) );
}


void PointSource::doInterpolationStep( wonder_frames_t blocksize )
{
    position.doInterpolationStep( blocksize );
}


PointSource::~PointSource()
{ }

//--- plane wave ----

DelayCoeff PlaneWave::calcDelayCoeff( const Speaker& speaker, const Angle& ang )
{
    Vector2D diff = speaker.getPos() - position.getCurrentValue();
    float cospsi  = ( ang.getNormal() * diff ) / diff.length();
    float delay   = diff.length() * cospsi;
    
    // factor is determined by angle between speaker normal and direction of the plane wave
    float factor = speaker.getNormal() * ang.getNormal();

    if( factor <= 0.0 )
        return DelayCoeff( 0.0, 0.0 );
    else
        return DelayCoeff( delay, factor * speaker.getCosAlpha() * twonderConf->planeComp );
}


DelayCoeff PlaneWave::getDelayCoeff( const Speaker& speaker )
{
    return calcDelayCoeff( speaker, angle.getCurrentValue());
}


DelayCoeff PlaneWave::getTargetDelayCoeff( const Speaker& speaker, wonder_frames_t blocksize )
{
    return calcDelayCoeff( speaker, angle.getTargetValue( blocksize ) );
}

/// NOTE: original also applies interpolation step to position?
void PlaneWave::doInterpolationStep( wonder_frames_t blocksize )
{
    position.doInterpolationStep( blocksize ); // re-added by MB; august 2010
    angle.doInterpolationStep( blocksize );
}


PlaneWave::~PlaneWave()
{
}
