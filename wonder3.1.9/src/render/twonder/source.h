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

#ifndef SOURCE_H
#define SOURCE_H


#include "angle.h"
#include "vector2d.h"
#include "interpolat.h"

class Speaker;
class DelayCoeff;

class Source 
{
public:
        virtual ~Source();

        virtual DelayCoeff getDelayCoeff( const Speaker& spk ) = 0;
        virtual DelayCoeff getTargetDelayCoeff( const Speaker& spk, wonder_frames_t blocksize ) = 0;

        virtual void doInterpolationStep( wonder_frames_t blocksize ) = 0;

        int getType()
        {
            return type; 
        }

        bool hasDopplerEffect()
        {
            return dopplerEffect; 
        }

        void setDopplerEffect( bool useDoppler )
        {
            dopplerEffect = useDoppler;
        }

        virtual void reset()
        { 
            type = 1;
            dopplerEffect = true;
        }



protected:
        int type; // 0 = planewave, 1 = point source
        bool dopplerEffect;
};

class PositionSource : public Source 
{
public:
        Interpolat< Vector2D > position;
        PositionSource( const Vector2D& position );

        virtual void reset()
        {
            position.setCurrentValue( Vector2D( 0.f, 0.f ) );
            position.setTargetValue ( Vector2D( 0.f, 0.f ) );
            Source::reset();
        }
};


class PointSource : public PositionSource
{
public:
        PointSource( const Vector2D& p ) : PositionSource( p )
        {
            type          = 1;
            dopplerEffect = true;
        }

        DelayCoeff getDelayCoeff( const Speaker& spk );
        DelayCoeff getTargetDelayCoeff( const Speaker& spk, wonder_frames_t blocksize );
        void       doInterpolationStep( wonder_frames_t blocksize );
        ~PointSource();

private:
        DelayCoeff calcDelayCoeff( const Speaker& spk, const Vector2D& vec );

        bool isFocused( const Vector2D& src ) const;
};

class PlaneWave : public PositionSource
{

public: 
        // A plane wave has an originating Position and a direction. The position is inherited from PositionSource.
        
        Interpolat< Angle > angle; 

        PlaneWave( const Vector2D& position, float angle ) : PositionSource( position ), angle( angle )
        {
            type          = 0;
            dopplerEffect = true;
        }

        DelayCoeff getDelayCoeff( const Speaker& spk );
        DelayCoeff getTargetDelayCoeff( const Speaker& spk, wonder_frames_t blocksize );
        void       doInterpolationStep( wonder_frames_t blocksize );
        ~PlaneWave();

        virtual void reset()
        {
            angle.setCurrentValue(  0.f );
            angle.setTargetValue(  0.f );
            PositionSource::reset();
        }

private:
        DelayCoeff calcDelayCoeff( const Speaker &spk, const Angle &angle );
};

#endif // SOURCE_H
