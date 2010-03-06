 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  *                                                                                   *
  *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
  *  http://swonder.sourceforge.net                                                   *
  *                                                                                   *
  *                                                                                   *
  *  (c) Copyright 2006-8                                                             *
  *  Berlin Institute of Technology, Germany                                          *
  *  Audio Communication Group                                                        *
  *  www.ak.tu-berlin.de                                                              *
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

#include "tracker_state.h"
#include "tracker_config.h"

#include <lo/lo.h>

#include <termios.h>
#include <fcntl.h>

#include <sstream>
#include <list>
#include <iostream>
#include <cstring> 
#include <cmath>
#include <cstdlib>
#include <cstdio>

using namespace std;

TrackerState::TrackerState()
{
}


void TrackerState::initItracker()
{
    trackerHandle = ISD_OpenTracker( ( Hwnd ) NULL, 0, FALSE, trackerConf->verbose );
   
    if( trackerHandle < 1 )
    {
        printf( "Failed to detect InterSense tracking device\n" );
        exit( EXIT_FAILURE );
    }
    else
    { 
        // Get tracker configuration info 
        ISD_GetTrackerConfig( trackerHandle, &tracker, trackerConf->verbose );
       
        //// fill ISD_STATION_INFO_TYPE structure with current station configuration 
        //if( tracker.TrackerType == ISD_PRECISION_SERIES )
        //{
        //    ISD_STATION_INFO_TYPE station;
        //    ISD_GetStationConfig( trackerHandle, &station, 1 , verbose );
        //    // XXX currently nothing is done with the acquired station information, 
        //    // I guess printing could be usefull
        //}
    }
}

   
void TrackerState::initPtracker()
{
    // open serial port
    if( ( trackerPort = open( "/dev/ttyS0", O_RDWR ) ) == -1 )
    {
        cerr << "Error, could not open serial port." << endl;
        exit( EXIT_FAILURE );
    }


    // get port attributes
    struct termios tio;
    if( int errorID = tcgetattr( trackerPort, &tio ) )
    {
        cerr << "Error " << errorID << ", could not get serial port attributes." << endl;
        exit( EXIT_FAILURE );
    }

    // set port attributes
    cfmakeraw( &tio );
    tio.c_cflag |= CLOCAL;
    tio.c_cflag |= CREAD;

    // set port speed
    speed_t newSpeed = B115200; 
    if( int errorID = cfsetispeed( &tio, newSpeed ) )
    {
        cerr << "Error " << errorID << ", could not set new serial port input speed." << newSpeed << "." << endl;
        exit( EXIT_FAILURE );
    }

    if( int errorID = cfsetospeed( &tio, newSpeed ) )
    {
        cerr << "Error " << errorID << ", could not set new serial port output speed to " << newSpeed << "." << endl;
        exit( EXIT_FAILURE );
    }

    // set port attributes
    // must be done after setting speed!
    if( int errorID = tcsetattr( trackerPort, TCSANOW, &tio ) )
    {
        cerr << "Error " << errorID << ", could not set new serial port attributes." << endl;
        exit( EXIT_FAILURE );
    }

    // this seems necessary to activate the serial port
    write( trackerPort, "C", 1 );

    fsync( trackerPort );
}


void TrackerState::getITrackerState( float* state )
{     
    if( ISD_GetCommInfo( trackerHandle, &tracker ) )
    {  
        ISD_TRACKER_DATA_TYPE data;
        ISD_GetData( trackerHandle, &data );    

        for( int i = 0; i < 3; ++i )
            state[ i ] = floor( data.Station[ 0 ].Orientation[ i ] * 10 + 0.5 ) / 10;
    }
}


void TrackerState::getPTrackerState( float* state )
{
    char c = 0;
    string line;

    //get new position from tracker
    while( c != '\n' )
    {
        //if( read( trackerPort, &c, 1 ) == 1 ) 
            //line += c;
        read( trackerPort, &c, 1 ); 
        line += c;
    }
   
   stringstream lineparse( line );
   
   float header;
   float x; 
   float y;
   float z;
   float pan;
   float tilt;
   float rot;
   
   lineparse >> header >> x >> y >> z >> pan >> tilt >> rot; 
   
   state[ 0 ] = floor( pan  * 10 + 0.5 ) / 10;
   state[ 1 ] = floor( tilt * 10 + 0.5 ) / 10;
   state[ 2 ] = floor( rot  * 10 + 0.5 ) / 10;
}


void TrackerState::sendTrackerPos( float* pos )
{
    int err = 0;

    for( std::list< OSCClient >::const_iterator it = trackerConf->clients.begin(); it != trackerConf->clients.end(); ++it )
    {
        OSCClient client = *it;
        if( client.sendPan && client.sendTilt && client.sendRot )
            err = lo_send( client.address, "/WONDER/tracker/move", "ifff", 0, pos[ 0 ], pos[ 1 ], pos[ 2 ]);
        else 
        {
            if( client.sendPan )
                err = lo_send( client.address, "/WONDER/tracker/move/pan",  "f", pos[ 0 ] );
            if( client.sendTilt )
                err = lo_send( client.address, "/WONDER/tracker/move/tilt", "f", pos[ 1 ] );
            if( client.sendRot )
                err = lo_send( client.address, "/WONDER/tracker/move/rot",  "f", pos[ 2 ] );
        }
        if( err == -1  ) 
            fprintf( stderr, "lo_send failure: %s\n", lo_address_errstr( client.address ) );
    }
}


void TrackerState::showTrackerPos( float* pos )
{
    if( trackerConf->trackerType == ITRACKER )
        printf( "%3.0fKbps %d R/s ", ( float ) ( &tracker )->KBitsPerSec, ( int )( &tracker )->RecordsPerSec );
   
    for( std::list< OSCClient >::const_iterator it = trackerConf->clients.begin(); it != trackerConf->clients.end(); ++it )
    {
        OSCClient client = *it;
        printf( "%7.2f %7.2f %7.2f ", pos[ 0 ] * ( int ) client.sendPan, pos[ 1 ] * ( int ) client.sendTilt, pos[ 2 ] * ( int ) client.sendRot );
        printf( "\r" );
        fflush( 0 );
    }
}


TrackerState::~TrackerState()
{
    if ( trackerConf->trackerType == ITRACKER )
        ISD_CloseTracker( trackerHandle );
    else
        close( trackerPort );
}
