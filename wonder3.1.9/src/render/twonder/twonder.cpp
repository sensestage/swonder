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

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <string>
#include <sstream>
#include <fstream>

#include "twonder_config.h"
#include "speaker.h"
#include "speakersegment.h"
#include "delayline.h"
#include "delaycoeff.h"
#include "source.h"
#include "jackppclient.h"
#include "rtcommandengine.h"
#include "oscin.h"
#include "timestamp.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>
#include <cstdio>
#include <csignal>



// forward declaration of global pointer to the jackClient
jackpp::Client* jackClient;


//--------------------------------SpkArray------------------------------------//

/**
 * @brief SpkArray holds all speakers in a Vector. 
 */
class SpkArray : public vector< Speaker* >
{
public:
        SpkArray( string filename );
        ~SpkArray();

		/// trunk has an extra constructor with just the float parameters
private:
        void addSpeakers( Segment* segment );
};



 ///
 /// @brief Constructs a SpkArray reading from file.
SpkArray::SpkArray( string filename ) 
{
    SegmentArray segarr( filename );

    vector< Segment* >::iterator segiter;

    for( segiter = segarr.segments.begin(); segiter != segarr.segments.end(); ++segiter )
        addSpeakers( *segiter );
}

///
/// @brief Adds a segment of speakers to the array..
void SpkArray::addSpeakers( Segment* segment )
{
    float    cosAlpha       = 1.0;
    Vector3D speakerPosition = segment->start;
    Vector3D speakerSpacing( 0.0, 0.0, 0.0 );

    if( segment->noSpeakers  > 1 )
    {
        speakerSpacing = ( segment->end - segment->start ) * ( 1.0 / ( segment->noSpeakers - 1 ) );

        // Calculate energy compensation for elevations of speakers ( i.e. speakerSpacing[2] != 0 )
        Vector2D xyVec( speakerSpacing[ 0 ], speakerSpacing[ 1 ] );
        float alpha = atanf( speakerSpacing[ 2 ] / xyVec.length() );
        cosAlpha   = cosf( alpha );
    }

    for( int i = 0; i < segment->noSpeakers; ++i )
    {
        stringstream spkname;
        spkname << "speaker" << size() + 1;

        Speaker* temp = new Speaker( spkname.str(),
                                     speakerPosition[ 0 ], speakerPosition[ 1 ], speakerPosition[ 2 ],
                                     segment->normal[ 0 ], segment->normal[ 1 ], segment->normal[ 2 ],
                                     cosAlpha ) ;
        push_back( temp );
        speakerPosition += speakerSpacing;
    }
}

SpkArray::~SpkArray()
{
    while( ! empty() )
    {
        delete back();
        pop_back();
    }
}

//----------------------------end of SpkArray---------------------------------//



//--------------------------------SourceAggregate-----------------------------//

//A Source with jackport, DelayLine and sourceType
/**
 * @brief Aggregates Source with jackport, DelayLine, sourceType.
 */ 
class SourceAggregate
{
public:
        SourceAggregate( string jackPortname, Vector2D initialPos );
        ~SourceAggregate();

        void reset()
        {
            source->reset();
            angle = 0.f;
        }

        DelayLine*    inputline;
        jackpp::Port* input;
        Source*       source;
        bool          active;
        float         angle; // for preserving angle when type of source is changed back and forth
};


SourceAggregate::SourceAggregate( string jackPortname, Vector2D initialPos )
{
    input = jackClient->registerPort( jackPortname, JACK_DEFAULT_AUDIO_TYPE, 
                                      JackPortIsInput, jackClient->getBufferSize() );

    inputline = new DelayLine( twonderConf->negDelayInit );
    source    = new PointSource( initialPos );
    active    = false;
    angle     = 0.0;
}


SourceAggregate::~SourceAggregate()
{
    delete inputline;
    delete source;
}

//----------------------------end of SourceAggregate--------------------------//



//--------------------------------SourceArray---------------------------------//

/**
 * @brief To hold all the (sound) sources.
 */
class SourceArray : public vector< SourceAggregate* >
{
public:
    SourceArray( int noSources );
    ~SourceArray();
};

///
/// @brief Constructs a SourceArray with a number of point sources at default coordinates (0,1). 
SourceArray::SourceArray( int noSources )
{
    for( int i = 0; i < noSources; ++i )
    {
        stringstream inputName;
        inputName << "input" << i;

        Vector2D defaultSourcePos( 0.0, 1.0 );
        SourceAggregate* temp = new SourceAggregate( inputName.str(), defaultSourcePos );
        push_back( temp ) ;
    }
}

SourceArray::~SourceArray()
{
    while( ! empty() )
    {
        delete back();
        pop_back();
    }
}

//----------------------------end of SourceArray------------------------------//



// global variables
SpkArray*        speakers;
SourceArray*     sources;
OSCServer*       oscServer;
RTCommandEngine* realtimeCommandEngine;



//--------------------------------MoveCommand---------------------------------//

/**
 * @brief Is a QCommand to move a PositionSource through space.
 */
class MoveCommand : public Command
{
public:
        MoveCommand( Vector2D& v, int id = 0, TimeStamp timestamp = 0 , int durationInSamples = 10240 ) : Command ( timestamp )
        { 
	    destination = v;
	    sourceId    = id; 
	    duration    = durationInSamples; ///NOTE: was previously divided through the jackclient buffer size?
	}

        void execute();

private:
        Vector2D     destination;
        unsigned int sourceId;
        unsigned int duration; // in samples
};


void MoveCommand::execute() 
{
    if( sourceId < sources->size() )
    {
        PositionSource* positionSource = dynamic_cast< PositionSource* >( sources->at( sourceId )->source );
        if( positionSource )
        {
            if( positionSource->getType() == 0 ) // is a plane wave
                positionSource->position.setCurrentValue( destination );
            else                                // is a point source
                positionSource->position.setTargetValue( destination , duration );
			  ///NOTE: had a check for (duration < 1 ? 1 : duration )
        }
    }
}

//----------------------------end of MoveCommand------------------------------//



//--------------------------------AngleCommand--------------------------------//

/**
 * @brief Is a QCommand to move a PlaneWave to a new angle.
 */
class AngleCommand : public Command
{
public:
        AngleCommand( float af, int id = 0, TimeStamp timestamp = 0, int durationInSamples = 10240 ): Command ( timestamp )
        {
	    angle     = af;
	    sourceId  = id; 
	    duration  = durationInSamples; ///NOTE: same as point source
	}

        void execute();

private:
        float        angle;
        unsigned int sourceId;
        unsigned int duration; // in samples
};


void AngleCommand::execute() 
{
    if( sourceId < sources->size() )
    {
        PlaneWave* planeWave = dynamic_cast< PlaneWave* > ( sources->at( sourceId )->source );
        if( planeWave )
        {
            planeWave->angle.setTargetValue( angle * M_PI * 2 / 360.0,  duration );
			///NOTE: same as point source
        }
    }
}

//----------------------------end of AngleCommand-----------------------------//



//--------------------------------TypeChangeCommand---------------------------//

/**
 * @brief Is a QCommand to change a sources type to another.
 */
class TypeChangeCommand : public Command 
{
public:
        TypeChangeCommand( int id, int type, TimeStamp ts = 0 );
        ~TypeChangeCommand();

        void execute();

private:
        unsigned int sourceId;
        unsigned int type;
        Source*      srcPtr;
};


TypeChangeCommand::TypeChangeCommand( int id, int t, TimeStamp timestamp ) : Command( timestamp )
{
        sourceId  = id;
        type      = t;

        PositionSource* temp = dynamic_cast< PositionSource* > ( sources->at( sourceId )->source );
        if( type == 1 )
        {
            srcPtr = new PointSource( temp->position.getCurrentValue() );
            srcPtr->setDopplerEffect( temp->hasDopplerEffect() );
        }
        else if( type == 0 ) 
        {
            srcPtr = new PlaneWave( temp->position.getCurrentValue(), sources->at( sourceId )->angle * M_PI * 2 / 360.0 );
            srcPtr->setDopplerEffect( temp->hasDopplerEffect() );
        }
        else
            srcPtr = NULL;
}


void TypeChangeCommand::execute() 
{

    if( sourceId < sources->size() ) 
    {
        Source* tmp = sources->at( sourceId )->source;

        sources->at( sourceId )->source = srcPtr;

        srcPtr = tmp;
    }
}


TypeChangeCommand::~TypeChangeCommand()
{
    if( srcPtr )
    {
        delete srcPtr;
        srcPtr = NULL;
    }
}

//----------------------------end of TypeChangeCommand------------------------//



//------------------------------DopplerChangeCommand--------------------------//

/**
 * @brief Is a QCommand to turn the Doppler effect on and off.
 */
class DopplerChangeCommand : public Command 
{
public:
        DopplerChangeCommand( int id, bool dopplerOn, TimeStamp ts = 0 );

        void execute();

private:
        unsigned int sourceId;
        bool         useDoppler;
};


DopplerChangeCommand::DopplerChangeCommand( int id, bool dopplerOn, TimeStamp timestamp ) : Command( timestamp )
{
        sourceId  = id;
        useDoppler = dopplerOn;
}


void DopplerChangeCommand::execute() 
{

    if( sourceId < sources->size() ) 
    {
        sources->at( sourceId )->source->setDopplerEffect( useDoppler );
    }
}

//----------------------------end of DopplerChangeCommand------------------------//



//----------------------------------OSC-handler-------------------------------//

// arguments of the handler functions
#define handlerArgs const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data

int oscSrcPositionHandler( handlerArgs )
{
    if( argv[ 0 ]->i >= twonderConf->noSources  ||  argv[ 0 ]->i < 0 )
        return -1;

	// if source is not active, ignore the command
    if( ! sources->at( argv[ 0 ]->i )->active )
        return 0;
    
    int   sourceId = argv[ 0 ]->i;
    float newX     = argv[ 1 ]->f;
    float newY     = argv[ 2 ]->f;
    float time     = 0.0;
    float duration = 0.0;

    // xxx legacy support for deprecated z coordinate xxxxxxxxxxxxxxxxxxx // 
    // note that time and duration are also flipped in the newer versions //
	//NOTE: this is where the z-coordinate could be introduced again for multi-z-level arrays
    if( argc == 6 )
    {
        time     = argv[ 4 ]->f;
        duration = argv[ 5 ]->f;
    }
    // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx //
    else if( argc == 5 )
    {
        duration = argv[ 3 ]->f;
        time     = argv[ 4 ]->f;
    }
    else if( argc == 4 )
    {
        duration = argv[ 3 ]->f;
    }


    if( twonderConf->verbose )
    {
        std::cout << "osc-position: src=" << sourceId
                  << " x="   << newX
                  << " y="   << newY
                  << " ts="  << time
                  << " dur=" << duration
                  << std::endl;
    }

    Vector2D newPos( newX, newY );

    MoveCommand* moveCmd = new MoveCommand( newPos, 
                                            sourceId,
                                            TimeStamp( time ), // conversion to sampletime happens in TimeStamp( float );
                                            ( int ) ( duration * twonderConf->sampleRate ) );
    realtimeCommandEngine->put( moveCmd );

    return 0;
}


int oscSrcAngleHandler( handlerArgs )
{
    if( argv[ 0 ]->i >= twonderConf->noSources  ||  argv[ 0 ]->i < 0 )
        return -1;

    if( ! sources->at( argv[ 0 ]->i )->active )
        return 0;

    int   sourceId  = argv[ 0 ]->i;
    float newAngle = argv[ 1 ]->f;
    float time      = 0.0;
    float duration  = 0.0;

    if( argc == 4 )
    {
        duration = argv[ 2 ]->f;
        time     = argv[ 3 ]->f;
    }
    else if( argc == 3 )
    {
        duration = argv[ 2 ]->f;
    }

    if( twonderConf->verbose )
    {
        std::cout << "osc-angle: src=" << sourceId
                  << " a="             << newAngle
                  << " ts="            << time
                  << " dur="           << duration
                  << std::endl;
    }

    AngleCommand* angleCmd = new AngleCommand( newAngle, sourceId, TimeStamp( time ), ( int ) ( duration * twonderConf->sampleRate ) );
	/// NOTE: why is time handled differently here?? in srcPos it is multiplied by the samplerate
    realtimeCommandEngine->put( angleCmd );

    // preserve angle information for type switching of sources if source is of type pointsource
    if( sources->at( sourceId )->source->getType() == 0 )
        sources->at( sourceId )->angle = newAngle;

    return 0;
}

/**
 * @brief Is a QCommand to change a sources type to another.
 */
int oscSrcTypeHandler( handlerArgs )
{
    if( argv[ 0 ]->i >= twonderConf->noSources  ||  argv[ 0 ]->i < 0  || argv[ 1 ]->i < 0 || argv[ 1 ]->i > 1 )
        return -1;

    if( ! sources->at( argv[ 0 ]->i )->active )
        return 0;

    int   sourceId = argv[ 0 ]->i;
    int   newType = argv[ 1 ]->i;
    float time     = 0.0;

    // if targeted source is already of the requested type, then nothing has to be done
    if( argv[ 1 ]->i == sources->at( sourceId )->source->getType() )
        return 0;

    if( argc == 3 )
        time = argv[ 2 ]->f;

    if( twonderConf->verbose ) 
    {
        std::cout << "osc-type-change: src=" << sourceId 
                  << " type="                 << newType 
                  << " ts="                   << time 
                  << std::endl;
    }

    TypeChangeCommand* typeCmd = new TypeChangeCommand( sourceId, newType, TimeStamp( time ) );
	/// NOTE: here the time is handled again differently??
    realtimeCommandEngine->put( typeCmd );

    return 0;
}

int oscSrcDopplerHandler( handlerArgs )
{
    if( argv[ 0 ]->i >= twonderConf->noSources  ||  argv[ 0 ]->i < 0 )
        return -1;

    if( ! sources->at( argv[ 0 ]->i )->active )
        return 0;

    int   sourceId   = argv[ 0 ]->i;
    bool  useDoppler = ( bool ) argv[ 1 ]->i;
    float time       = 0.0;

    if( argc == 3 )
        time = argv[ 2 ]->f;

    if( twonderConf->verbose ) 
    {
        std::cout << "osc-doppler-change: src=" << sourceId 
                  << " doppler="                << useDoppler
                  << " ts="                     << time 
                  << std::endl;
    }

    DopplerChangeCommand* dopplerCmd = new DopplerChangeCommand( sourceId, useDoppler, TimeStamp( time ) );
	/// NOTE: time is handled again as with the type change
    realtimeCommandEngine->put( dopplerCmd );

    return 0;
}


int oscReplyHandler( handlerArgs )
{
    if( twonderConf->verbose )
    {
        cout << "[V-OSCServer] reply to: " << &argv[ 0 ]->s
             << " state="                  <<  argv[ 1 ]->i 
             << " msg="                    << &argv[ 2 ]->s
             << endl;
    }

    return 0;
}


int oscSrcActivateHandler( handlerArgs ) 
{
    if( argv[ 0 ]->i >= twonderConf->noSources  ||  argv[ 0 ]->i < 0 )
        return 0;

    if( twonderConf->verbose )
        std::cout << "osc-activate " << argv[ 0 ]->i << std::endl;

    sources->at( argv[ 0 ]->i )->active = true;

    return 0;
}


int oscSrcDeactivateHandler( handlerArgs ) 
{
    if( argv[ 0 ]->i >= twonderConf->noSources  ||  argv[ 0 ]->i < 0 )
        return 0;

    if( twonderConf->verbose )
        std::cout << "osc-deactivate " << argv[ 0 ]->i << std::endl;

    sources->at( argv[ 0 ]->i )->active = false;
    sources->at( argv[ 0 ]->i )->reset();

    return 0;
}


int oscPingHandler( handlerArgs )
{
    lo_send( twonderConf->cwonderAddr, "/WONDER/stream/render/pong", "i", argv[ 0 ]->i );

    return 0;
}


int oscNoSourcesHandler( handlerArgs )
{
    // this handler is only allowed to be called once and only with a valid
    // value at startup, so ignore further messages
    static bool noSourcesIsSet = false;

    if( ! noSourcesIsSet )
    {
        if( argv[ 0 ]->i > 0 )
        {
            twonderConf->noSources = argv[ 0 ]->i;
            noSourcesIsSet           = true;
            sources = new SourceArray( twonderConf->noSources );
        }
    }
    return 0;
}

int oscRenderPolygonHandler( handlerArgs )
{
    // parse incoming points

    // argv[ 0 ] is the roomname, drop it, we don't need that
    // get number of points 
    int noPoints = argv[ 1 ]->i;

    // iterate over all points and store them
    for( int i = 1; i <= noPoints; ++i)
    {
        twonderConf->renderPolygon.push_back( Vector3D( argv[ ( i * 3 ) - 1 ]->f,
                                                        argv[   i * 3       ]->f,
                                                        argv[ ( i * 3 ) + 1 ]->f ) );
    }
    
    
    // TODO:calculate elevation of speakers
    //slope = ( elevationZ2 - elevationZ1 ) / ( elevationY2 - elevationY1 );

    return 0;
}

int oscElevationHandler( handlerArgs )
{
    // parse incoming points

    // argv[ 0 ] is the roomname, drop it, we don't need that
    // get number of points 

    // iterate over all points and store them
    twonderConf->elevationY1 = argv[ 1 ]->f;
    twonderConf->elevationZ1 = argv[ 2 ]->f;
    twonderConf->elevationY2 = argv[ 3 ]->f;
    twonderConf->elevationZ2 = argv[ 4 ]->f;
    
    // TODO:calculate elevation of speakers
    twonderConf->slope = ( twonderConf->elevationZ2 - twonderConf->elevationZ1 ) / ( twonderConf->elevationY2 - twonderConf->elevationY1 );
  
    if ( fabs( twonderConf->slope ) > 1e-3 ){
	twonderConf->hasSlope = true;
    } else {
	twonderConf->hasSlope = false;
    }
    
    return 0;
}

int oscGenericHandler( handlerArgs )
{
    cout << endl << "[twonder]: received unknown osc message: " << path << endl;
    return 0;
}

//------------------------------end of OSC-Handler---------------------------//



//-------------------------------Audio processing----------------------------//

/// this is the function called by the JACK thread for realtime audio processing
int process( jack_nframes_t nframes, void* arg )
{
    static int printor = jackClient->getSampleRate() / jackClient->getBufferSize();  // for debugging

    // process incoming commands
    realtimeCommandEngine->evaluateCommands( ( wonder_frames_t ) 10 ); 

    // process audio
    
    // fill input lines
    SourceArray::iterator sourcesIt;
    for( sourcesIt = sources->begin(); sourcesIt != sources->end(); ++sourcesIt )
    {
        float* inputbuf = ( float* ) ( *sourcesIt )->input->getBuffer( nframes );
        ( *sourcesIt )->inputline->put( inputbuf, nframes );
    }
    
    // for debugging
    if( twonderConf->nonRTDebug )
    {
        if( printor-- )
        { 
        }
        else 
            printor = jackClient->getSampleRate() / jackClient->getBufferSize();  
    }

    // for each speaker calculate the delay coefficients for each source
    // and then render the samples produced by these sources into the outputbuffer
    int i = 0; // just for printor debug to count speakers
    SpkArray::iterator speakersIt;
    for( speakersIt = speakers->begin(); speakersIt != speakers->end(); ++speakersIt )
    {
        float* outbuf = ( float* ) ( *speakersIt )->port->getBuffer( nframes );

        // fill the accumulation buffer with zeros
        // XXX: is this efficient??? maybe use memset instead?
        for( unsigned int k= 0; k < nframes; ++k )
            outbuf[ k ] = 0.0;
        
        int j = 0; // just for printor debug to count sources
        for( sourcesIt = sources->begin(); sourcesIt != sources->end(); ++sourcesIt )
        {
            DelayCoeff c  = ( *sourcesIt )->source->getDelayCoeff      ( **speakersIt );
            DelayCoeff c2 = ( *sourcesIt )->source->getTargetDelayCoeff( **speakersIt, nframes );

            if( ! ( *sourcesIt )->active )
            {
                c.setFactor ( 0.0 );
                c2.setFactor( 0.0 );
            }
            
            // debug info
            if( ! printor )
            {
                cerr<<" speaker/source " << i << "/" << j << endl;
                cerr << "current coeff: ";
                c.print();
                cerr << " target coeff: ";
                c2.print();
            }
            
            /// doppler on/off is now a boolean, no longer a threshold, as in trunk
            if( ! ( *sourcesIt )->source->hasDopplerEffect() )
            {
                if( ! printor )
                   cerr << "using fadejump (no doppler)" << endl;

                ( *sourcesIt )->inputline->getFadej( c, c2, outbuf, nframes, DelayLine::dB3 );
            }
            else
            {
                if( ! printor )
                   cerr << "using interpolation (doppler)" << endl;

                ( *sourcesIt )->inputline->getInterp( c, c2, outbuf, nframes );
            }
            ++j; // printor debug
        }
        ++i; // printor debug
    }

    for( sourcesIt = sources->begin(); sourcesIt != sources->end(); ++sourcesIt )
        ( *sourcesIt )->source->doInterpolationStep( nframes );


    return 0;
}

//---------------------------end of audio processing-------------------------//


std::string pathPidfile;

int startDaemon()
{
    pid_t pid;

    pid = fork();

	///NOTE: version in trunk checks for CHILD, not 0...
    if( pid  < 0 )
	exit( EXIT_FAILURE );

    if( pid > 0 )
	exit( EXIT_SUCCESS );

    ///NOTE: version in trunk checks for ERROR not 0...
    if( setsid() < 0 )
        exit( EXIT_FAILURE  );

	/// change to root in file system
    if( ( chdir( "/" ) ) < 0 ) 
        exit( EXIT_FAILURE  );

    umask( 0 );

    for( int i = sysconf( _SC_OPEN_MAX ); i > 0; i-- )
        close( i );

    /// check if the directory for pid file exist
    ifstream fin;
    string path = "/tmp/wonder";
    fin.open( path.c_str() , ios_base::in );

    if( !fin.is_open() )
    {
        /// create /tmp/wonder directory if it does not exist yet
        /// the sticky: anyone is able to create files, but
        /// not able to delete anyone elses files but their own
        int error = mkdir( path.c_str(), mode_t(S_IRWXU | S_IRWXG | S_IRWXO | S_ISVTX));
        if( error != 0 )
            return 1;   
    }

    /// open file stream to write the pid in
    string name;
    ostringstream spid;

    /// get pid of parent process
    spid << getpid();

    if( twonderConf->user == NULL )
        pathPidfile = path + "/twonder." + spid.str() + ".pid";
    else
        pathPidfile = path + "/twonder." + ( string ) twonderConf->user + ".pid";

    ofstream pidFile( pathPidfile.c_str() );
    pidFile << spid.str();
    pidFile.close();

    /// write stdin to /dev/null
    freopen("/dev/null", "r", stdin );

    /// write stdout and stderr to the log file     
    freopen( "/var/log/wonder.log", "w", stdout );
    freopen( "/var/log/wonder.log", "w", stderr );

    return 0;
}

int cleanDaemon()
{
    /// delete the pid file
    if( twonderConf->daemon )
    {   
        if( remove( pathPidfile.c_str() ) != 0 )
            return 1;   
    }    

    return 0;
}


void exitCleanupFunction()
{
    cleanDaemon();
    if( jackClient )
        jackClient->deactivate();

    if( oscServer )
    {
        oscServer->stop();
        delete oscServer;
        oscServer = NULL;
    }
    if( twonderConf )
    {
        delete twonderConf;
        twonderConf = NULL;
    }
    if( speakers )
    {
        delete speakers;
        speakers = NULL;
    }
    if( sources )
    {
        delete sources;
        sources = NULL;
    }
    if( realtimeCommandEngine )
    {
        delete realtimeCommandEngine;
        realtimeCommandEngine = NULL;
    }
    if( jackClient )
    {
       delete jackClient;
       jackClient = NULL;
    }
}


void signalHandler( int sig )
{
    cleanDaemon();
    exit( EXIT_SUCCESS );
}       


int main( int argc, char** argv )
{
    // register shutdown cleanup function
    atexit( exitCleanupFunction );

    // read the twonder configuration file
    twonderConf = new TwonderConfig( argc, argv );
    int retVal = twonderConf->readConfig(); 

    if( retVal != 0 )
    {
        std::cerr << "Error parsing twonder configfile !!!" << std::endl;
        switch( retVal )
        {
            case 1:
                std::cerr << "configfile " << twonderConf->twonderConfigfile << " does not exist. " << std::endl;
                break;
            case 2:
                std::cerr << "dtd file" << " could not be found. " << std::endl;
                break;
            case 3:
                std::cerr << "configfile " << twonderConf->twonderConfigfile << " is not well formed. " << std::endl;
                break;
            case 4:
                std::cerr << "libxml caused an exception while parsing " << twonderConf->twonderConfigfile << std::endl;
                break;
            default:
                std::cerr << " an unkown error occurred, sorry. " << endl;
                break;
        }
        exit( EXIT_FAILURE );
    }
    
    // add signal handlers
    signal( SIGQUIT, signalHandler );
    signal( SIGTERM, signalHandler );
    signal( SIGHUP,  signalHandler );
    signal( SIGINT,  signalHandler );  

    // daemonize
    if( twonderConf->daemon )
    {
        if( startDaemon() != 0 )
        {
            cleanDaemon();
            return 1;
        }
    }

    // setup the jack client 
    jackClient = new jackpp::Client( twonderConf->jackName );
    if( ! jackClient->connect() )
    {
        cleanDaemon();        
        exit( EXIT_FAILURE );
    }
  
    // get the current sampleRate from jack 
    twonderConf->sampleRate = jackClient->getSampleRate();
    TimeStamp::initSampleRate( twonderConf->sampleRate );

    // init fade factors in delaylines
    DelayLine::initFadeBuffers( jackClient->getBufferSize() );
    
    // set up outputs (i.e. speakers ), with acces to jackClient
    Speaker::setJackClient( jackClient );

    // read the speaker configuration file
    try
    {
        if( twonderConf->verbose )
            std::cout << "using speakers configfile: " << twonderConf->speakersConfigfile << std::endl;

        speakers = new SpkArray( twonderConf->speakersConfigfile );
    }
    catch( std::exception& e )
    {
        std::cerr << "could not read speaker configuration file " // << twonderConf->speakersConfigfile
                 << ", exiting now... " << endl;
        cleanDaemon();        
        exit( EXIT_FAILURE );
    }

    // start the command engine
    realtimeCommandEngine = new RTCommandEngine();

    // start OSCServer and register messagehandler
    try
    {
        oscServer = new OSCServer( twonderConf->listeningPort );
    }
    catch( OSCServer::EServ )
    {
        cerr << "[twonder] Could not create osc server, maybe a server ( using the same port ) is already running?" << endl;                   
        cleanDaemon();        
        exit( EXIT_FAILURE );
    }

    // multiple entries for one message (e.g. type) are just for convenience and will use a value of 0 for timestamp and/or duration
    oscServer->addMethod( "/WONDER/reply",                     "sis",    oscReplyHandler );
    oscServer->addMethod( "/WONDER/source/activate",           "i",      oscSrcActivateHandler );
    oscServer->addMethod( "/WONDER/source/deactivate",         "i",      oscSrcDeactivateHandler );
    oscServer->addMethod( "/WONDER/source/position",           "iff",    oscSrcPositionHandler );
    oscServer->addMethod( "/WONDER/source/position",           "ifff",   oscSrcPositionHandler );
    oscServer->addMethod( "/WONDER/source/position",           "iffff",  oscSrcPositionHandler );
    oscServer->addMethod( "/WONDER/source/position",           "ifffff", oscSrcPositionHandler );
    oscServer->addMethod( "/WONDER/source/type",               "ii",     oscSrcTypeHandler );
    oscServer->addMethod( "/WONDER/source/type",               "iif",    oscSrcTypeHandler );
    oscServer->addMethod( "/WONDER/source/angle",              "if",     oscSrcAngleHandler );
    oscServer->addMethod( "/WONDER/source/angle",              "iff",    oscSrcAngleHandler );
    oscServer->addMethod( "/WONDER/source/angle",              "ifff",   oscSrcAngleHandler );
    oscServer->addMethod( "/WONDER/source/dopplerEffect",      "ii",     oscSrcDopplerHandler );
    oscServer->addMethod( "/WONDER/source/dopplerEffect",      "iif",    oscSrcDopplerHandler );
    oscServer->addMethod( "/WONDER/global/maxNoSources",       "i",      oscNoSourcesHandler );
    oscServer->addMethod( "/WONDER/global/renderpolygon",      NULL,     oscRenderPolygonHandler );
    oscServer->addMethod( "/WONDER/global/elevation",          NULL,     oscElevationHandler );    
    oscServer->addMethod( "/WONDER/stream/render/ping",        "i",      oscPingHandler );
    oscServer->addMethod( NULL,                                NULL,     oscGenericHandler );  
    oscServer->start();

    // connect to cwonder
    lo_send( twonderConf->cwonderAddr, "/WONDER/stream/render/connect", "s", twonderConf->name.c_str() );

    // wait for cwonder to send setup data
    // exit, if it does not work
    while( twonderConf->noSources == 0 )
    {
        static int timeoutCounter = 0;
        sleep( 1 );

        if( timeoutCounter == 5 )
        {
            cerr << "could not establish connection to cwonder, exiting now... " << endl;
            cleanDaemon();        
            exit( EXIT_FAILURE );
        }
        timeoutCounter++;
    }

    // tell jack which function does the audio processing and start the client
    jackClient->setProcessCallback( process );
    jackClient->activate();

    // event loop
    if( twonderConf->daemon )
    {
        while( 1 )
            sleep( 10 );
    } 
    else
    {
        while( 1 )
        {
            // implement some keyboard input code here, if
            // you don't want to use OSC
            sleep( 10 );
        }
    }
    
    /// NOTE: does this make sense here?
//     if( twonderConf->daemon )
//         cleanDaemon();        

    // cleanup before exiting 
    exitCleanupFunction();

    return 0;
}
