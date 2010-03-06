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

#include <lo/lo.h>
#include "keyio.h"
#include "tracker_state.h"
#include "tracker_config.h"
#include "isense.h"
#include "oscin.h"

#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

using namespace std;
        
OSCServer* oscServer;

// arguments of the handler functions
#define handlerArgs const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data

int oscTrackerOmitHandler( handlerArgs )
{
    if( argv[ 0 ]->i < 0 ) 
        return -1;

    trackerConf->omit = argv[ 0 ]->i;

    if( trackerConf->verbose )
        std::cout << endl << "Tracker omit = " << trackerConf->omit << std::endl;

    return 0;
}

int oscTrackerResetHandler( handlerArgs )
{
    // did not receive a '1' via osc /WONDER/tracker/reset, so  do nothing	
    if( argv[ 0 ]->i != 1 ) 
        return -1;
    
    // reset tracker:
    // userdata is pointer to 1st element of array w. 2 pointers to 1st element of 2 floatarrays[3]
    float **pp_reset_args=(float **)user_data; 
  
    float *p_state  = *pp_reset_args;  
    float *p_offset = *(pp_reset_args+1);

    // now overwrite "offset" w. "state"  (memcpy(*dest, *src, ,bytes))
    memcpy( p_offset, p_state,  3* sizeof( float ) );
    
    if( trackerConf->verbose) 
    {
        printf( "New offset: %7.2f  %7.2f  %7.2f   \n\r", *p_offset, *(p_offset+1), *(p_offset+2) );
        std::cout << endl << "Tracker resetted via OSC." <<  std::endl;
    }
    return 0;
}

int oscGenericHandler( handlerArgs )
{
    cout << endl << "[tracker]: received unknown osc message: " << path << endl;
    return 0;
}


int main( int argc, char** argv )
{
    float state    [ 3 ] = { 0.f, 0.f, 0.f };
    float state_old[ 3 ] = { 0.f, 0.f, 0.f };
    float position [ 3 ] = { 0.f, 0.f, 0.f };
    float offset   [ 3 ] = { 0.f, 0.f, 0.f };
    float zerovec  [ 3 ] = { 0.f, 0.f, 0.f };

    // workaround for (i)tracker jumping to wrong sign occasionally (e.g. from -18 to 18)
    float max_diff = 20.0;

    trackerConf = new TrackerConfig( argc, argv );

    // read the tracker configuration file
    int retVal = trackerConf->readConfig();

    if( retVal != 0 )
    {
        std::cerr << "Error parsing tracker configfile !!!" << std::endl;
        switch( retVal )
        {
            case 1:
                std::cerr << "configfile " << trackerConf->trackerConfigfile << " does not exist. " << std::endl;
            break;
            case 2:
                std::cerr << "dtd file" << " could not be found. " << std::endl;
            break;
            case 3:
                std::cerr << "configfile " << trackerConf->trackerConfigfile << " is not well formed. " << std::endl;
            break;
            case 4:
                std::cerr << "libxml caused an exception while parsing " << trackerConf->trackerConfigfile << std::endl;
            break;
            default:
            std::cerr << " an unkown error occurred, sorry. " << endl;
        }
        exit( EXIT_FAILURE );
    }

    // osc server for incoming messages
    try
    {
        oscServer = new OSCServer( trackerConf->listeningPort );
    }
    catch( OSCServer::EServ )
    {
        cerr << "[tracker] Could not create server, maybe the server( using the same port ) is already running?" << endl;                   
        exit( EXIT_FAILURE );
    }


    // collect references for oscTrackerResetHandler
    float *p_reset_args[2];
    p_reset_args[0]=&state[0];
    p_reset_args[1]=&offset[0];

    oscServer->addMethod( "/WONDER/tracker/omit", "i", oscTrackerOmitHandler );
    oscServer->addMethod( "/WONDER/tracker/reset", "i", oscTrackerResetHandler,p_reset_args);
    oscServer->addMethod( NULL, NULL, oscGenericHandler );  
    oscServer->start();
    
    // print current configuration info
    cout << endl;
    trackerConf->print();
    cout << endl;

    //open connection to the tracker and initialise the tracker
    TrackerState* tracker = new TrackerState();

    if( trackerConf->trackerType == ITRACKER ) 
        tracker->initItracker();
    else
        tracker->initPtracker();

    // print key commands
    cout <<  "Keyboard controls:"         << endl
         <<  "q -> quit tracker"          << endl
         <<  "o -> reset tracker"         << endl
         <<  "p -> change pan-sign"       << endl
         <<  "t -> change tilt-sign"      << endl
         <<  "r -> change rot-sign"       << endl
         <<  "v -> toggle verbose on/off" << endl;


    // for latency testing
    int noTimes   = trackerConf->testPoints;
    int diffIndex = 0;
    timeval times[ noTimes ];

    int key = 0;
    int signPan  = 1;
    int signTilt = 1;
    int signRot  = 1;

    int messageCounter = 0;

    nonblock( 1 );   
    bool keepRunning = true;
    while( keepRunning )
    {
        ++messageCounter;
        
        // preserve old position for comparison
        // memcpy ( dest *, src *, size )
        memcpy ( state_old, state, sizeof( state_old ) );
        
        // get new position
        if( trackerConf->trackerType == ITRACKER )
            tracker->getITrackerState( state );
        else
            tracker->getPTrackerState( state );

        // discard messages according to the omit parameter, omit of 0 means use every message
        if( ( messageCounter % ( trackerConf->omit + 1 ) ) == 0  )
        {
            messageCounter = 0;

            // do timing of incoming tracker position data, should be around 8-9ms
            if( trackerConf->latencyTestMode )
            {
                gettimeofday( &times[ diffIndex++ ], NULL );
                if( diffIndex == noTimes )
                    break;
            }

            // send new tracker position via OSC but only if the position changed and the amount of change is valid.
            // always send data when in latency test mode to sync amount of timedata with receiving osc client
            // XXX: currently only pan and tilt are checked, but rot is set and send as well
            if(    ( ( ( fabs( state[ 0 ] - state_old[ 0 ] ) > 0.0 ) | ( fabs( state[ 1 ] - state_old[ 1 ] ) > 0.0 ) )
                   &&   ( fabs( state[ 0 ] - state_old[ 0 ] ) < max_diff )
                   &&   ( fabs( state[ 1 ] - state_old[ 1 ] ) < max_diff ) )  
                || trackerConf->latencyTestMode )
            {
                // correct offsets
                position[ 0 ] = state[ 0 ] - offset[ 0 ];
                position[ 1 ] = state[ 1 ] - offset[ 1 ];
                position[ 2 ] = state[ 2 ] - offset[ 2 ];
                
                if     ( position[ 0 ] >  180 ) position[ 0 ] -= 360 ;
                else if( position[ 0 ] < -180 ) position[ 0 ] += 360 ;
                
                if     ( position[ 1 ] >  90 ) position[ 1 ] -= 180 ; 
                else if( position[ 1 ] < -90 ) position[ 1 ] += 180 ;
                
                if     ( position[ 2 ] >  180 ) position[ 2 ] -= 360 ;
                else if( position[ 2 ] < -180 ) position[ 2 ] += 360 ;
              
                //sign correction
                position[ 0 ] *= signPan;
                position[ 1 ] *= signTilt;
                position[ 2 ] *= signRot;
              
                //send new position via OSC
                tracker->sendTrackerPos( position );
              
                // print new position
                if( trackerConf->verbose )
                    tracker->showTrackerPos( position );
            }
        }
      
        // itracker is much to busy while "usleep( 1 );" 'though ptracker is fine with it
        usleep( trackerConf->wait );

        key = kbhit();
        //wait for keyboard hit
        if( key != 0 )
        {
            switch( fgetc( stdin ) )
            {
                //reset tracker to 0.0, 0.0, 0.0    
                case 'o':
                case 'O': 
                     // memcopy(*dest, *src, ,bytes)
                     memcpy( offset, state, sizeof( offset ) );
                    printf( "New Offset: %7.2f   %7.2f   %7.2f \n\r", offset[ 0 ], offset[ 1 ], offset[ 2 ] );
                    tracker->showTrackerPos( zerovec  );
                    break;
                //quit tracker    
                case 'q':
                case 'Q':
                    keepRunning = false;
                    printf( "exiting...\n" );
                    break;
                case 'p':
                case 'P':
                    signPan *= -1;
                    break;
                case 't':
                case 'T':
                    signTilt *= -1;
                    break;
                case 'r':
                case 'R':
                    signRot *= -1;
                    break;
                case 'v':
                case 'V':
                    trackerConf->verbose = ! trackerConf->verbose;
                default:
                    break;
            }
        }
    }
    nonblock( 0 );


    // evaluate latency test data
    if( trackerConf->latencyTestMode )
    {
        // open file to which results should be written
        ofstream resultsFile;
        if( trackerConf->writeLatencyFile )
        {
            resultsFile.open( trackerConf->latencyTestFileName.c_str(), ios::out );
            if( ! resultsFile )
            {
                cerr << "Error, could not open " << trackerConf->latencyTestFileName << " for writing the latency test results. " << endl;
                trackerConf->writeLatencyFile = false;
            }
        }

        double minDiff = LONG_MAX; // just set a really large value we can use on initial comparison
        double maxDiff = 0;        // just set a really small value we can use on initial comparison
        int    noDiffs = diffIndex - 1;

        double runningMean     = 0;
        double sumDiffsSquared = 0;

        // calculate min, max, mean and standard deviation
        for( int i = 0; i < noDiffs; ++i )
        {
            double  sDiff =  times[ i + 1 ].tv_sec  - times[ i ].tv_sec;
            double usDiff =  times[ i + 1 ].tv_usec - times[ i ].tv_usec;
            
            double diffInMs = usDiff / 1000 + sDiff * 1000;

            if( trackerConf->writeLatencyFile ) 
                resultsFile << diffInMs << endl;

            if( diffInMs < minDiff && diffInMs != 0 )
                minDiff = diffInMs;

            if( diffInMs > maxDiff )
                maxDiff = diffInMs;

            sumDiffsSquared += ( diffInMs * diffInMs );

            runningMean += ( diffInMs - runningMean ) / ( i + 1 ); 
        }
        double standardDeviation = sqrt( (  sumDiffsSquared / noDiffs ) - ( runningMean * runningMean ) );

        // print results
        int printPrecision = 3;
        int printWidth     = 8;
        cout << fixed << endl
             << "Latency testing results: "                                                                                               << endl
             << "mean     intervall is " << setw( printWidth ) << setprecision( printPrecision ) << runningMean       << " milliseconds " << endl
             << "standard deviation is " << setw( printWidth ) << setprecision( printPrecision ) << standardDeviation << " milliseconds " << endl
             << "minimal  intervall is " << setw( printWidth ) << setprecision( printPrecision ) << minDiff           << " milliseconds " << endl
             << "maximal  intervall is " << setw( printWidth ) << setprecision( printPrecision ) << maxDiff           << " milliseconds " << endl
             << "last omit setting was " << trackerConf->omit                                                                             << endl;

        // write results to file
        if( trackerConf->writeLatencyFile ) 
        {
            resultsFile << endl
                        << "(all data is in milliseconds)"            << endl
                        << "mean               " << runningMean       << endl
                        << "standard deviation " << standardDeviation << endl
                        << "min                " << minDiff           << endl
                        << "max                " << maxDiff           << endl << endl
                        << "last omit setting: " << trackerConf->omit << endl;
            resultsFile.close();
            cout << "Latency test file written to " << trackerConf->latencyTestFileName << endl;
        }
    }

    // clean up 
    delete oscServer;

    return 0;
}
