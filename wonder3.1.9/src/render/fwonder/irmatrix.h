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

#ifndef IRMATRIX_H
#define IRMATRIX_H

#include <lo/lo.h>
#include <utility>

class ImpulseResponse;


class IRMatrix
{

public:
    IRMatrix();
    ~IRMatrix();
	
protected:
    ImpulseResponse* findBestIR( float x, float y );

    std::pair< int,int > getEmptyPosition       ( int x, int y );
    std::pair< int,int > getDisposableIRPosition( int x, int y, int directionX, int directionY );

    ImpulseResponse*     getIRAt     ( int x, int y );
    bool                 isIRLoadedAt( int x, int y );

    void addIR( ImpulseResponse* IR, int x, int y, bool isDynamicIR );
    void removeIR( int x, int y );


    // these are only local duplicates of what is already 
    // stored in the global fwonder config
    int staticCacheXResolution;
    int staticCacheYResolution;
    int dynamicCacheXResolution;
    int dynamicCacheYResolution;
    int dynamicCacheXRadius;
    int dynamicCacheYRadius;

    int xMin;
    int yMin;
    int xMax;
    int yMax;

    // need mutexlock
    int xCurrent;
    int yCurrent;

    int width; 
    int height;

    int noLoadedStaticIRs;
    int noLoadedDynamicIRs;

    pthread_mutex_t mutex;

private:
    void /*IRMatrix&*/ operator = ( const IRMatrix& other );
    IRMatrix( const IRMatrix& other);

    //int possibleNeigh;
    int cacheRadiusX;
    int cacheRadiusY;

    ImpulseResponse* currentIR;

    // this represents a 2D array of IRs but arranged as one linear array of adjacent rows starting with the bottom row (smallest elevation)
    // rowWidth = ( azimuthStop - azimuthStart + 1 )
    // i.e. the IR of azimuthStart     | elevationStart     is at IRMatrix[ 0 ]
    //      the IR of azimuthStart + 1 | elevationStart     is at IRMatrix[ 1 ]
    //      the IR of azimuthStart     | elevationStart + 1 is at IRMatrix[ rowWidth ]
    //      the IR of azimuthStart     | elevationStart + 2 is at IRMatrix[ 2 * rowWidth ]
    // (see configfile for details on azimuth and elevation)
    ImpulseResponse** theIRMatrix;
};

#endif
