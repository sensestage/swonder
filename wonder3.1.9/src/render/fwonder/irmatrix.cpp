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

#include "irmatrix.h"

#include <iomanip>
#include <iostream>
#include <cmath>
#include <cfloat>


#include "fwonder_config.h"
#include "impulseresponse.h"

using namespace std;


IRMatrix::IRMatrix()
{
    lo_send( fwonderConf->qfwonderAddr, "/WONDER/qfwonder/reset", "" ); 

    staticCacheXResolution = fwonderConf->staticCacheXResolution;
    staticCacheYResolution = fwonderConf->staticCacheYResolution;

    dynamicCacheXResolution = fwonderConf->dynamicCacheXResolution;
    dynamicCacheYResolution = fwonderConf->dynamicCacheYResolution;

    dynamicCacheXRadius = fwonderConf->dynamicCacheXRadius;
    dynamicCacheYRadius = fwonderConf->dynamicCacheYRadius;

    xMin = fwonderConf->sourceAzimuthStart;
    xMax = fwonderConf->sourceAzimuthStop;
    yMin = fwonderConf->sourceElevationStart;
    yMax = fwonderConf->sourceElevationStop;

    cacheRadiusX = fwonderConf->dynamicCacheXRadius;
    cacheRadiusY = fwonderConf->dynamicCacheYRadius;

    // dimensions of the matrix
    width  = xMax - xMin + 1;
    height = yMax - yMin + 1;

    noLoadedStaticIRs  = 0;
    noLoadedDynamicIRs = 0;

    theIRMatrix = new ImpulseResponse* [ width * height ];

    for( int i = 0; i < width * height; ++i )
        theIRMatrix[ i ] = NULL;

    xCurrent = 0;
    yCurrent = 0;
}


IRMatrix::~IRMatrix()
{
    if( theIRMatrix )
    {
        for( int i = 0; i < width * height; ++i )
        {
            if( theIRMatrix[ i ] )
            {
                delete theIRMatrix[ i ];
                theIRMatrix[ i ] = NULL;
            }
        }
        delete[] theIRMatrix;
        theIRMatrix = NULL;
    }
}


ImpulseResponse* IRMatrix::getIRAt( int x, int y )
{
    // check for invalid position requests
    if( x < xMin || x > xMax || y < yMin || y > yMax )
        return NULL;

    return theIRMatrix[ ( x - xMin ) + ( y - yMin ) * width ];
}


bool IRMatrix::isIRLoadedAt( int x, int y )
{
    // check for invalid position requests
    if( x < xMin || x > xMax || y < yMin || y > yMax )
        return false;
    
    return ( theIRMatrix[ ( x - xMin ) + ( y - yMin ) * width ] != NULL );
}


//XXX: find out what really happens here...
// it seems that static IRs may be omitted, because the stepsize is determined by the resolution of the dynamic cache
ImpulseResponse* IRMatrix::findBestIR( float x, float y ) 
{
    //XXX: maybe implement this search in a differnt manner, e.g. like the search for empty IR positions
    ImpulseResponse* bestIR = NULL;

    float minimalDistance = FLT_MAX;

    int   stepX = fwonderConf->dynamicCacheXResolution;
    int   stepY = fwonderConf->dynamicCacheYResolution;

    pthread_mutex_lock( &mutex );
        for( int i = ( int ) x - stepX; i <= ( int ) x + stepX; ++i )
        {
            for( int j = ( ( int ) y - stepY ); j <= ( int ) y + stepY; ++j )
            {
                if( isIRLoadedAt( i, j ) && ( ( i % stepX ) == 0 ) && ( ( j % stepY ) == 0 ) ) 
                {
                    float distance = ( ( x - i ) * ( x - i ) + ( y - j ) * ( y - j ) );
                    if( distance < minimalDistance )
                    {
                        minimalDistance = distance;

                        xCurrent = i;
                        yCurrent = j;

                        bestIR  = getIRAt( i, j );
                    }
                }
            }
        }
    pthread_mutex_unlock( &mutex );

    if( fwonderConf->IRVerbose )
        cout <<  "Current IR in use:" << setw( 6 ) << xCurrent  << ", " << setw( 6 ) << yCurrent << "  pan: " 
             << setw( 6 ) << x << "  tilt: " << setw( 6 ) << y << "\r" << endl << flush;

    // send currently used ir to qfwonder
    lo_send( fwonderConf->qfwonderAddr , "/WONDER/qfwonder/currentIR", "ii", xCurrent, yCurrent ); 

    return bestIR;
}


pair< int, int > IRMatrix::getEmptyPosition( int x , int y )
{
    // find the nearest empty position (i.e no IR loaded)
    // search is starting at the given x|y position
    pthread_mutex_lock  ( &mutex );
        for( int j = 0; j <= dynamicCacheYRadius && ( y -j ) >= yMin && ( y + j ) <= yMax; j += dynamicCacheYResolution )
        {
            for( int i = 0; i <= dynamicCacheXRadius && ( x - i ) >= xMin && ( x + i ) <= xMax ; i += dynamicCacheXResolution )
            {
                if( ! isIRLoadedAt(         x + i, y + j ) )
                {
                    pthread_mutex_unlock( &mutex );
                    return pair< int, int >( x + i, y + j );
                }
                else if( ! isIRLoadedAt(    x + i, y - j ) )
                {
                    pthread_mutex_unlock( &mutex );
                    return pair< int, int >( x + i, y - j );
                }
                else if( ! isIRLoadedAt(    x - i, y + j ) )
                {
                    pthread_mutex_unlock( &mutex );
                    return pair< int, int >( x - i, y + j );
                }
                else if( ! isIRLoadedAt(    x - i, y - j ) )
                {
                    pthread_mutex_unlock( &mutex );
                    return pair< int, int >( x - i, y - j );
                }
            }
        }
    pthread_mutex_unlock( &mutex );

    // this is an indicator for "no emtpy position found", check for this return value in the calling function
    return pair< int, int >( xMax + 1, yMax + 1 );
}


pair< int, int > IRMatrix::getDisposableIRPosition( int x, int y, int directionX, int directionY )
{  
    // linewise checking for disposable IRs, always starting in the corner behind the current IR (indicated by direction values)
    // dynamically loaded IRs outside of the cache radius are disposable
    // statically loaded IRs are never disposable, so check the static cache resolution
    if( directionY < 0 ) // tilting downwards
    {
        if( directionX > 0 ) // panning right
 	{
            for( int ix = xMin; ix <= xMax; ++ix )     // starting left
                for( int iy = yMax; iy >= yMin; --iy ) // starting at the top
                    if( ! ( ( ( ix % staticCacheXResolution ) == 0 ) && ( ( iy % staticCacheYResolution ) == 0 ) ) ) 
                        if( isIRLoadedAt( ix, iy ) && ix != xCurrent && iy != yCurrent )
                            if( ( (int) abs( (float) (x - ix) ) > cacheRadiusX ) || ( (int) abs( (float) (y - iy) ) > cacheRadiusY ) )
                                return pair< int, int >( ix, iy );
        }
 	else // panning left or no panning
 	{
            for( int ix = xMax; ix >= xMin; --ix )     // starting right
                for( int iy = yMax; iy >= yMin; --iy ) // starting at the top
                    if( ! ( ( ( ix % staticCacheXResolution ) == 0 ) && ( ( iy % staticCacheYResolution ) == 0 ) ) ) 
                        if( isIRLoadedAt( ix, iy ) && ix != xCurrent && iy != yCurrent )
                            if( ( (int) abs( (float) (x - ix) ) > cacheRadiusX ) || ( (int) abs( (float) (y - iy) ) > cacheRadiusY ) )
                                return pair< int, int >( ix, iy );
 	}
    }
    else // tilting upwards or no tilting
    {
        if( directionX > 0 ) // panning right
 	{
            for( int ix = xMin; ix <= xMax; ++ix )     // starting left
                for( int iy = yMin; iy <= yMax; ++iy ) // starting at the bottom
                    if( ! ( ( ( ix % staticCacheXResolution ) == 0 ) && ( ( iy % staticCacheYResolution ) == 0 ) ) ) 
                        if( isIRLoadedAt( ix, iy ) && ix != xCurrent && iy != yCurrent )
		            if( ( (int) abs( (float) (x - ix) ) > cacheRadiusX ) || ( (int) abs( (float) (y - iy) ) > cacheRadiusY ) )
			        return pair< int, int >( ix, iy );
 	}
        else// panning left or no pannning
 	{
            for( int ix = xMax; ix >= xMin; --ix )     // starting right
                for( int iy = yMin; iy <= yMax; ++iy ) // starting at the bottom
                    if( ! ( ( ( ix % staticCacheXResolution ) == 0 ) && ( ( iy % staticCacheYResolution ) == 0 ) ) ) 
                        if( isIRLoadedAt( ix, iy ) && ix != xCurrent && iy != yCurrent )
                            if( ( (int) abs( (float) (x - ix) ) > cacheRadiusX ) || ( (int) abs( (float) (y - iy) )> cacheRadiusY ) )
                                return pair< int, int >( ix, iy );
        }
    }

    // this is an indicator for "no disposable IR found", check for this return value in the calling function
    return pair< int, int >( xMax + 1, yMax + 1 );
}


void IRMatrix::addIR( ImpulseResponse* IR, int x, int y, bool isDynamicIR )
{
    // delete passed in IR if it is invalid
    if( x < xMin  ||  x > xMax  || y < yMin  ||  y > yMax ) 
    {
	cerr << "IR Index (" << x << "|" << y << ") of new IR out of range! " << endl;
        if( IR )
        {
            delete IR;
            IR = NULL;
        }
	return;
    }

    IR->xPos = x;
    IR->yPos = y;

    //pthread_mutex_lock  ( &mutex );
        theIRMatrix[ ( x - xMin ) + ( y - yMin ) * width ] = IR;
    //pthread_mutex_unlock( &mutex );

    // keep track of number of loaded IRs an send that to qfwonder
    if( isDynamicIR )
    {
        ++noLoadedDynamicIRs;
        lo_send( fwonderConf->qfwonderAddr, "/WONDER/qfwonder/numLoadedIRs", "ii", noLoadedDynamicIRs, isDynamicIR );
    }
    else
    {
        ++noLoadedStaticIRs;  
        lo_send( fwonderConf->qfwonderAddr, "/WONDER/qfwonder/numLoadedIRs", "ii", noLoadedStaticIRs, isDynamicIR );
    }

    // send info about loaded IR to qfwonder
    lo_send( fwonderConf->qfwonderAddr, "/WONDER/qfwonder/IRLoaded", "iiii", x, y, 1, isDynamicIR ); 
}


void IRMatrix::removeIR( int x, int y )
{
    if( x < xMin  ||  x > xMax  ||  y < yMin  ||  y > yMax ) 
    {
        cerr << "IR index of removeIR out of range !!!" << endl;
        return;
    }

    ImpulseResponse* removeThisIR = theIRMatrix[ ( x - xMin ) + ( y - yMin ) * width ];
      
    pthread_mutex_lock( &mutex );
        if( removeThisIR )
        {
            delete removeThisIR;
            removeThisIR = NULL;
            --noLoadedDynamicIRs;
    pthread_mutex_unlock( &mutex );

            //Communicate with the qfwonder
            lo_send( fwonderConf->qfwonderAddr, "/WONDER/qfwonder/numLoadedIRs", "ii",   noLoadedDynamicIRs, true );
            lo_send( fwonderConf->qfwonderAddr, "/WONDER/qfwonder/IRLoaded",     "iiii", x, y, 0, true ); 
            theIRMatrix[ ( x - xMin ) + ( y - yMin ) * width ] = NULL;
        }
        else
    pthread_mutex_unlock( &mutex );
}
