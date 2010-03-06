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

#include "irmatrixmanager.h"
#include "fwonder_config.h"
#include "impulseresponse.h"

#include <sstream>
#include <iostream>
#include <utility>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::pair;


IRMatrixManager::IRMatrixManager( int IRPartitionSize, int noIRPartitions, string IRPath, string tailPath ) : IRPartitionSize( IRPartitionSize ),
                                                                                                              maxNoPartitions( noIRPartitions ),
                                                                                                              IRPath( IRPath ),
                                                                                                              tailPath( tailPath )
{
    targetPosX = 0;
    targetPosY = 0;
    directionX = 0;
    directionY = 0;

    pthread_mutex_init( &mutex, NULL );

    loadStaticIRs();
    // XXX: this does not take into account the resolution of the dynamic cache, so this value may be too big when using resolutions
    //      other than 1. Find a formula to keep the radius of dynamically loaded IRs constant!
    dynamicCacheMaxNoIRs = ( 2 * dynamicCacheXRadius + 1 ) * ( 2 * dynamicCacheYRadius + 1 );
}

ImpulseResponse* IRMatrixManager::getBestIR()
{
    return findBestIR( targetPosX, targetPosY );
}



void IRMatrixManager::loadStaticIRs()
{
    if( fwonderConf->IRVerbose )
        cout << "Loading BRIRs from: " << IRPath << endl;

    for( int y = ( yMin / staticCacheYResolution ) * staticCacheYResolution; y <= yMax; y += staticCacheYResolution )
    {
        for( int x = ( xMin / staticCacheXResolution ) * staticCacheXResolution; x <= xMax; x += staticCacheXResolution )
        {
	    ImpulseResponse* IR = new ImpulseResponse( getFileName( x, y ), IRPartitionSize, maxNoPartitions, -1, 0, ImpulseResponse::STATIC_IR );

	    SndfileHandle file( getFileName( x,y ).c_str() );
	    
	    if( IR && file.channels() )
		addIR( IR, x, y, false );
	    else
            {
                cerr << "Error! Could not load " << getFileName( x,y ).c_str() << endl;
                delete IR;
                exit( EXIT_FAILURE );
            }
	}
    }
}


string IRMatrixManager::getFileName( int x, int y )
{
    int  maxFileNameLength = 20;
    char xNumber[ maxFileNameLength ];
    char yNumber[ maxFileNameLength ];
    
    snprintf( yNumber, maxFileNameLength, "%c%02d", y < 0 ? 'N' : 'P' ,abs( y ) );
    snprintf( xNumber, maxFileNameLength, "%c%02d", x < 0 ? 'N' : 'P' ,abs( x ) );

    stringstream fileName;
    fileName << IRPath << yNumber << "_" << xNumber << "0.wav";

    return fileName.str();
}


string IRMatrixManager::getTailName() 
{
    stringstream tailName;
    tailName << tailPath << fwonderConf->tailName << ".wav";

    return tailName.str();
}


void IRMatrixManager::setPanAndTilt( float pan, float tilt ) 
{
    if( pan >= xMin && pan <= xMax && tilt >= yMin && tilt <= yMax )
    {
        pthread_mutex_lock  ( &mutex );
            float oldX = targetPosX;
            float oldY = targetPosY;

            targetPosX = pan;
            targetPosY = tilt;

            directionX = ( pan  > oldX ) - ( oldX > pan  ) + directionX * ( oldX == pan );
            directionY = ( tilt > oldY ) - ( oldY > tilt ) + directionY * ( oldY == tilt );
        pthread_mutex_unlock( &mutex );
    }
}


void IRMatrixManager::setPan( float pan ) 
{
    if( pan >= xMin && pan <= xMax )
    {
        pthread_mutex_lock  ( &mutex );
            float oldX = targetPosX;
            targetPosX = pan;
            directionX = ( pan > oldX ) - ( oldX > pan ) + directionX * ( oldX == pan );
        pthread_mutex_unlock( &mutex );
    }
}

void IRMatrixManager::setTilt( float tilt ) 
{
    if( tilt >= yMin && tilt <= yMax )
    {
        pthread_mutex_lock  ( &mutex );
            float oldY = targetPosY;
            targetPosY = tilt;
            directionY = ( tilt > oldY ) - ( oldY > tilt ) + directionY * ( oldY == tilt );
        pthread_mutex_unlock( &mutex );
    }
}


bool IRMatrixManager::manage()
{
    pthread_mutex_lock  ( &mutex );
        int posX = xCurrent;
        int posY = yCurrent;

        int dirX = directionX;
        int dirY = directionY;
    pthread_mutex_unlock( &mutex );

    // find an IR that can be deleted
    if( noLoadedDynamicIRs >= dynamicCacheMaxNoIRs )
    {
        pair< int, int > removePos = getDisposableIRPosition( posX, posY, dirX, dirY );
        int x = removePos.first;
        int y = removePos.second;
        
        // xMax + 1 means "no disposable IR found"
        if( x != ( xMax + 1 )  )
            removeIR( x, y );
        else
        {
            // signal: not done managing
            return false;
        }
    }

    // try to load new IR if not already at the maximum of allowed dynamic IRs
    if( noLoadedDynamicIRs < dynamicCacheMaxNoIRs )
    {
        pair< int,int > loadPos = getEmptyPosition( posX, posY );
        int x = loadPos.first;
        int y = loadPos.second;

        //xMax + 1 means "no emtpy position found"
        if( x != ( xMax + 1 )  )
        {
            ImpulseResponse* IR = new ImpulseResponse( getFileName( x, y ), IRPartitionSize, maxNoPartitions, -1, 0, ImpulseResponse::DYNAMIC_IR );

            // don't add IR if it has no channels (not sure how this might happen though...)
            if( IR->getNoChannels() )
                addIR( IR, x, y, true );
            else
                delete IR;
        }
        else
        {
            // signal: not done managing
            return false;
        }
    }
    // signal: done managing
    return true;
}


ImpulseResponse* IRMatrixManager::getTail( int tailPartitionSize, int noTailPartitions, int maxIRLength )
{
    if( fwonderConf->IRVerbose )
        cout << "Loading tail: " << getTailName().c_str() << endl;

    // XXX: shouldn't there be a 1 added to maxIRLength/tailPartitionSize to make up for the integer division???
    ImpulseResponse* IR = new ImpulseResponse( getTailName(), tailPartitionSize, noTailPartitions,
                                               ( maxIRLength / tailPartitionSize ), IRPartitionSize, ImpulseResponse::TAIL_IR );
    SndfileHandle file( getTailName().c_str() );

    if( IR && file.channels() )
	return IR;
    else
    {
	cerr << "Error! Could not load " << getTailName().c_str() << endl;
        exit( EXIT_FAILURE );
    }
    
    return 0;
}
