
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

#include <utility>
#include <string>
#include <sstream>
#include <fstream>

#include "speaker.h"
#include "delayline.h"

#include "source.h"

#include "jackppclient.h"

#include "rtcommandengine.h"
#include "osc.h"
#include "twonder_globalconfig.h"
#include "spkarray.h"

#include "swonder_config.h"
#include "global_array.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#define CHILD 0
#define ERROR -1

#define VERSION 0.1


using namespace std;

jackpp::Client *jackclient;

float ilen( Vector2D a ) { return a.len(); }

/**
 * @brief SpkArray holds all speakers in a Vector. 
 */
class SpkArray : public vector<Speaker> {
    private:
	void add_segment_speakers( Segment *seg );
    public:
	SpkArray( float x, float y, float dx, float dy, float nx, float ny, int n );
	SpkArray( string filename );
};

 ///
 /// @brief Constructs a SpkArray using a given number of speakers in a row with given distance. 
 SpkArray::SpkArray( float x, float y, float dx, float dy, float nx, float ny, int n ) {
    
    for( int i=0; i<n; i++ ) {
	stringstream spkname;
	spkname << "speaker" << i;

	push_back( Speaker( spkname.str(), x,y, 0.0, nx, ny, 0.0, 1.0 ) );
	x += dx;
	y += dy;
    }
}

 ///
 /// @brief Constructs a SpkArray reading from file.
SpkArray::SpkArray( string filename ) {
    SegArray segarr( filename );

    vector<Segment *>::iterator segiter;

    for( segiter = segarr.segments.begin(); segiter != segarr.segments.end(); segiter++ ) {
	add_segment_speakers( *segiter );
    }
}

///
/// @brief Adds a segment of speakers to the array..
void SpkArray::add_segment_speakers( Segment *seg ) {

    float cos_alpha=1.0;
    Vector3D posX = seg->start;
    Vector3D dpos( 0.0, 0.0, 0.0 );
    if( seg->numspeak  > 1 ) {
	dpos = (seg->end - seg->start) * (1.0/(seg->numspeak-1));

	// Calculate the energy compensation for the KNICK (bend)..
	Vector2D xyvec( dpos[0], dpos[1] );
	float alpha = atanf( dpos[2]/xyvec.len() );
	cos_alpha = cosf( alpha );
    }

    Vector2D norm( seg->norm[0], seg->norm[1] );

    for( int i=0; i<seg->numspeak; i++ ) {
	
	stringstream spkname;
	spkname << "speaker" << size()+1;

	push_back( Speaker( spkname.str(), posX[0], posX[1], posX[2], norm[0], norm[1], 0.0, cos_alpha ) );
	posX = posX + dpos;
    }

}

/**
 * @brief Aggregates Source with jackport, DelayLine, source_type.
 */ 
class SourceAggregate {
    private:
	static jackpp::Client *jackclient;

    public:
	static void setJackClient( jackpp::Client *jc ) { jackclient = jc; }

	DelayLine *inputline;
	jackpp::Port *input;
	Source *src;
	bool mute;

	SourceAggregate( string jack_portname, string source_type_name, Vector2D initial_pos );
	~SourceAggregate();
};

jackpp::Client *SourceAggregate::jackclient;

SourceAggregate::SourceAggregate( string jack_portname, string source_type_name, Vector2D initial_pos ) {
    
    input = jackclient->registerPort( jack_portname, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, jackclient->getBufferSize() );

    inputline = new DelayLine(conf->negdelay_init);

    src = new PointSource( initial_pos );
    mute = 0;
}

SourceAggregate::~SourceAggregate() {
    //delete src;
    //delete inputline;
    //delete input;
}

/**
 * @brief To hold all the (sound) sources.
 */
class SourceArray : public vector<SourceAggregate> {
    public:
	SourceArray( int n );
};

///
/// @brief Constructs a SourceArray with a number of point sources at default coordinates (0,-2). 
SourceArray::SourceArray( int n ) {
    for( int i=0; i<n; i++ ) {
	stringstream input_name;
	input_name << "input" << i;

	push_back( SourceAggregate( input_name.str(), "point", Vector2D( 0,-2 ) ));
	//x += dx;
	//y += dy;
    }
}

// TODO:
//
// + make the Sources use the Interpolat.
// + Try to determine a common base class for sources.
// + evalate whether rtti is the right thing.
//
// + design new Command Class which integrates well with OSC.
//
// + port the jamin OSC ladspa.
//
// - implement roompolygon.
//

//jackpp::Port *input;
DelayLine *inputline;
SpkArray *spkarray;
SourceArray *src_array;

//PointSource *src;

RTCommandEngine *rteng;

OSCServer *oscserv;
SwonderConfig *sconf;

float xoffset, yoffset, zoffset;
float rotZ, rotZax, rotZay;

/**
 * @brief Is a QCommand to move a PositionSource through space.
 */
class MoveCommand : public QCommand {
    public:
	Vector2D vec;
	int source;
	int duration;
	void execute();
	MoveCommand( Vector2D &v, int s=0, TimeStamp ts=0, int dur = 10240 ) { vec = v; source = s; stamp = ts; duration = dur/jackclient->getBufferSize(); }
};

void MoveCommand::execute() {
	//cout << "got move: (" << mcmd->vec[0] << "," <<  mcmd->vec[1] << ")" << endl;
	if( source < src_array->size() ) {
		PositionSource *psrc = static_cast<PositionSource *>( src_array->at(source).src );
		if( psrc )
			psrc->pos.setVal( vec, duration < 1 ? 1 : duration );
	}
}

/**
 * @brief Is a QCommand to move a PlaneWave to a new angle.
 */
class AngleCommand : public QCommand {
    public:
	float anglef;
	int source;
	int duration;
	void execute();
	AngleCommand( float af, int s=0, TimeStamp ts=0, int dur = 10240 ) { anglef = af; source = s; stamp = ts; duration = dur/(jackclient->getBufferSize()); }
};

void AngleCommand::execute() {
	//cout << "got move: (" << mcmd->vec[0] << "," <<  mcmd->vec[1] << ")" << endl;
	if( source < src_array->size() ) {
		PlaneWave *psrc = dynamic_cast<PlaneWave *>( src_array->at(source).src );
		if( psrc )
			psrc->angle.setVal( anglef*M_PI*2/360.0, duration < 1 ? 1 : duration );
	}
}

/**
 * @brief Is a QCommand to change a sources type to another.
 */
class TypeChangeCommand : public QCommand {
    public:
	int source_num;
	float angle;
	int type;

	Source *src_ptr;

	void execute();
	TypeChangeCommand( int s, int t, float a, TimeStamp ts=0 );
	~TypeChangeCommand();
};

TypeChangeCommand::TypeChangeCommand( int s, int t, float a, TimeStamp ts ) {

	source_num = s;
	type = t;
	angle = a*M_PI*2/360.0;  //a*M_2_PI/360.0;
	stamp = ts;

	if( conf->verbose ) {
		Angle aa( angle );
		Vector2D aan = aa.getN();
		cout << "angeln= " <<  a << " " << angle << " " << aan[0] << " " << aan[1] << endl;
	}

	if( type == 1 ) {

		src_ptr = new PointSource( Vector2D( 0,-1.5 ) );

	} else if( type == 0 ) {

		src_ptr = new PlaneWave( Vector2D( 0, -1.5 ), angle );

	} else {
		src_ptr = NULL;
	}
}

void TypeChangeCommand::execute() {
	//cout << "got move: (" << mcmd->vec[0] << "," <<  mcmd->vec[1] << ")" << endl;
	if( source_num < src_array->size() ) {
		Source *tmp = src_array->at(source_num).src;

		src_array->at( source_num ).src = src_ptr;
		src_ptr = tmp;
	}
}

TypeChangeCommand::~TypeChangeCommand() {
	if( src_ptr )
		delete src_ptr;
}

int osc_move_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    Vector2D new_pos( argv[1]->f, argv[2]->f );
    
    if( conf->verbose )
	    cout << "osc-move: src=" << argv[0]->i << " x=" << argv[1]->f << " y=" << argv[2]->f << endl;
    //if( new_pos[1] < 20.0 ) {
	MoveCommand *mcmd = new MoveCommand( new_pos, argv[0]->i, 0, 10240 );
	rteng->put( mcmd );
    //}
    return 0;
}

int osc_src_position_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    Vector2D new_pos( argv[1]->f, argv[2]->f );

    if( conf->verbose )
	    cout << "osc-move: src=" << argv[0]->i << " x=" << argv[1]->f << " y=" << argv[2]->f << " ts=" << argv[4]->i << " dur=" << argv[5]->i << endl;

    //if( new_pos[1] < 20.0 ) {
	MoveCommand *mcmd = new MoveCommand( new_pos, argv[0]->i, TimeStamp( argv[4]->i ), argv[5]->i );
	rteng->put( mcmd );
    //}
    return 0;
}

int osc_nonintern_src_position_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    Vector2D new_pos( argv[1]->f, argv[2]->f );

    if( conf->verbose )
	    cout << "osc-move: src=" << argv[0]->i << " x=" << argv[1]->f << " y=" << argv[2]->f << " ts=" << argv[4]->f << " dur=" << argv[5]->f << endl;

	MoveCommand *mcmd = new MoveCommand( new_pos, argv[0]->i, TimeStamp( (int) (argv[4]->f * (float)jackclient->getSampleRate())  ), (int) (argv[5]->f * (float) jackclient->getSampleRate()) );

	rteng->put( mcmd );
    return 0;
}

int osc_src_angle_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    float new_ang( argv[1]->f );

    if( conf->verbose )
	    cout << "osc-angle: src=" << argv[0]->i << " a=" << argv[1]->f << " ts=" << argv[2]->i << " dur=" << argv[3]->i << endl;

	AngleCommand *acmd = new AngleCommand( new_ang, argv[0]->i, TimeStamp( argv[2]->i ), argv[3]->i );
	rteng->put( acmd );

    return 0;
}

int osc_src_type_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {
	int source_num = argv[0]->i;
	int new_type = argv[1]->i;
	float angle = argv[2]->f;

	int time = argv[3]->i;

	if( conf->verbose ) {
		cout << "osc-type-change: src=" << source_num << " type=" << new_type << " angle=" << angle << "ts= " << time << endl;
	}

	if( source_num < conf->num_sources ) {
		TypeChangeCommand *chcmd = new TypeChangeCommand( source_num, new_type, angle, time );
		rteng->put( chcmd );
	}

}

int osc_tracker_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    if( conf->verbose )
	cout << "osc-tracker:  x=" << argv[0]->f << " y=" << argv[1]->f << " z=" << argv[2]->f << endl;

    conf->gtrans.rotZ =  argv[0]->f;
}

int osc_fadejump_thres_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    if( conf->verbose )
	cout << "osc-fadejump-thres " << argv[0]->i << endl;

    for( int i=0; i < conf->num_sources; i++ ) {
	conf->fadejump_threshold->at(i) =  argv[0]->i;
    }
}

int osc_src_fadejump_thres_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    if( conf->verbose )
	cout << "osc-fadejump-thres " << argv[0]->i << " = " << argv[1]->i << endl;

    if( argv[0]->i < conf->num_sources )
	conf->fadejump_threshold->at( argv[0]->i ) = argv[1]->i;
}

int osc_negdelay_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    if( conf->verbose )
	cout << "osc-negdelay" << argv[0]->f << endl;

    for( int i=0; i < conf->num_sources; i++ ) {
	src_array->at(i).inputline->set_negdelay( argv[0]->f );
    }
}

int osc_src_negdelay_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    if( conf->verbose )
	cout << "osc-negdelay " << argv[0]->i << " = " << argv[1]->f << endl;

    if( argv[0]->i < conf->num_sources )
	src_array->at( argv[0]->i ).inputline->set_negdelay( argv[1]->f );
}

int osc_src_mute_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    if( conf->verbose )
	cout << "osc-mute " << argv[0]->i << "= " << argv[1]->i << endl;

    if( argv[0]->i < conf->num_sources )
	src_array->at( argv[0]->i ).mute = argv[1]->i;
}

int osc_ping_handler( const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data ) {

    lo_send( sconf->cwonder_addr, "/WONDER/stream/render/pong", "i", argv[0]->i );
}

int
process (jack_nframes_t nframes, void *arg) {

    static int printor = 100;
    // Process incoming Commands

    // FIXME: Use a real TimeStamp

    if( printor-- ) { } else { printor=100; }

    rteng->eval_cmds( 10 ); //jackclient->getFrameTime() );


    // Process audio.
    // ===========================================================================
    
    // Fill Input lines..
    SourceArray::iterator src_it;
    for( src_it = src_array->begin(); src_it != src_array->end(); src_it++ ) {
	float *inputbuf = (float *) src_it->input->getBuffer( nframes );
	src_it->inputline->put( inputbuf, nframes );
    }

    
    
    // Calc delay coeffs for sources, and render em into the outputbuffers.
    SpkArray::iterator it;
    for( it = spkarray->begin(); it != spkarray->end(); it++ ) {

	float *outbuf = (float *) it->port->getBuffer( nframes );
	for( int i=0; i<nframes; i++ )
	    outbuf[i] = 0.0;

	
	int i=0;
	for( src_it = src_array->begin(); src_it != src_array->end(); src_it++ ) {
	    DelayCoeff c = src_it->src->getDelayCoeff( *it );
	    DelayCoeff c2 = src_it->src->getTargetDelayCoeff( *it );

	    if( src_it->mute ) {
		c.setFactor( 0.0 );
		c2.setFactor( 0.0 );
	    }
	    
	    if( conf->nonrt_debug ) {
		if( !printor )
		    c.print();
	    }
	   


	    //src_it->inputline->get( c, outbuf, nframes );

	    int diff = abs(c2.getSampleDelay(0) - c.getSampleDelay(0));
	    
	    if( diff > (conf->fadejump_threshold->at(i)) ) {
		src_it->inputline->get_fadej( c, c2, outbuf, nframes );
	    } else {
		src_it->inputline->get_interp( c, c2, outbuf, nframes );
	    }
	    i++;

	}

    }

    for( src_it = src_array->begin(); src_it != src_array->end(); src_it++ ) {
	src_it->src->applyInterpolatTarget();
    }
    conf->gtrans_old = conf->gtrans;

    return 0;
}

void start_daemon()
{
    int i;
    pid_t pid;

    if((pid = fork()) != CHILD) exit(0);

    if(setsid() == ERROR)
        exit(0);

    chdir("/");

    umask(0);

    for(i=sysconf(_SC_OPEN_MAX); i>0; i--)
        close(i);

    ofstream pid_file( "/var/run/twonder.pid" );
    pid_file << getpid();
    pid_file.close();

    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);

    //ofstream *log_file = new ofstream( "/var/log/twonder.log" );
    //cout = *log_file;
    //cerr = *log_file;

}

void atexitus( void ) {
    jackclient->deactivate();
    unlink( "/var/run/twonder.pid" );
}

int main( int argc, char **argv ) {

    conf = new TWonderGlobalConfig( argc, argv );
    sconf = new SwonderConfig();
    if( sconf->readconfig( conf->swonder_conf ) != 0 ) {
	cerr << "Error parsing config !!!" << endl;
	exit( 20 );
    }

    garray = new GlobalArray();
    if( garray->readconfig( conf->global_array ) != 0 ) {
	cerr << "Error parsing global array config !!!" << conf->global_array << endl;
	exit( 20 );
    }

cout << "reference line at: " << garray->reference << endl;


#if 0
    {
	Interpolat<Angle> ainter ( 1.0 );
	int i;
	cout << fmod( -1.0, 2*M_PI ) << endl;

	Angle a(-1.0 );
	Angle b(3.0);

	cout << (float) (a+b) << endl; 
	cout << (float) (a) << endl; 
	cout << (float) (a) << endl; 
	cout << (float) (b) << endl; 
	
	ainter.setVal( -1.0, 3 );
	    cout << (float) ainter.getVal() << endl;
	for( i=0; i<5; i++ ) {
	    ainter.applyTargetVal();
	    cout << (float) ainter.getVal() << endl;
	}
	ainter.setVal( 2.0, 3 );
	    cout << (float) ainter.getVal() << endl;
	for( i=0; i<5; i++ ) {
	    ainter.applyTargetVal();
	    cout << (float) ainter.getVal() << endl;
	}
    }
#endif

    if( conf->daemon ) {
	start_daemon();
    }


    jackclient = new jackpp::Client( conf->jack_name );
    jackclient->connect();

    Speaker::setJackClient( jackclient );
    SourceAggregate::setJackClient( jackclient );

    rteng = new RTCommandEngine();

    string spkarray_conf_name = string( conf->config_path ) + "spkarray.xml";
    if( conf->verbose ) {
	cout << "using spkarray configfile: " << spkarray_conf_name << endl;
    }

    //spkarray = new SpkArray( -10,0, 0.1,0, 0,-1, 24 );
    spkarray = new SpkArray( spkarray_conf_name );
    Vector2D the_pos( 0, -1.5 );

    src_array = new SourceArray( conf->num_sources );

    jackclient->setProcessCallback( process );
    jackclient->activate();

    atexit( atexitus );

    oscserv = new OSCServer(conf->portname);
    oscserv->add_method( "move", "iff", osc_move_handler );
    oscserv->add_method( "twonder/move", "iff", osc_move_handler );
    oscserv->add_method( "/twonder/move", "iff", osc_move_handler );
    oscserv->add_method( "/WONDERINTERN/source/position", "ifffii", osc_src_position_handler );
    oscserv->add_method( "/WONDER/source/position", "ifffff", osc_nonintern_src_position_handler );
    oscserv->add_method( "/WONDERINTERN/source/type", "iifi", osc_src_type_handler );
    oscserv->add_method( "/WONDERINTERN/source/angle", "ifii", osc_src_angle_handler );
    oscserv->add_method( "/tracker", "fff", osc_tracker_handler );
    oscserv->add_method( "/WONDER/global/fadejump_threshold", "i", osc_fadejump_thres_handler );
    oscserv->add_method( "/WONDER/source/fadejump_threshold", "ii", osc_src_fadejump_thres_handler );
    oscserv->add_method( "/WONDER/global/max_negdelay", "f", osc_negdelay_handler );
    oscserv->add_method( "/WONDER/source/max_negdelay", "if", osc_src_negdelay_handler );
    oscserv->add_method( "/WONDER/source/mute", "ii", osc_src_mute_handler );
    oscserv->add_method( "/WONDER/stream/render/ping", "i", osc_ping_handler );
    oscserv->start();

    lo_send( sconf->cwonder_addr, "/WONDER/stream/render/connect", "" );

    int source=0;
    if( conf->daemon ) {
    	while( 1 ) sleep( 10 );
    } else {
	    while( 1 ) {
		    string cmd;
		    cin >> cmd;


		    if( cmd == "q" ) break;
		    if( cmd == "" ) break;

		    if( cmd[0] == 's' ) {
			    cmd.erase( cmd.begin() );
			    stringstream sstrea(cmd);
			    sstrea >> source;
			    cout << "Now Controlling Source Nr. " << source << endl;
			    continue;
		    }
		    if( cmd[0] == 'p' ) {
			    conf->nonrt_debug = true;
			    continue;
		    }
		    if( cmd[0] == 'o' ) {
			    conf->nonrt_debug = false;
			    continue;
		    }
		    if( cmd == "l" ) the_pos[0] -= 1.0;
		    if( cmd == "r" ) the_pos[0] += 1.0;
		    if( cmd == "L" ) the_pos[0] -= 10.0;
		    if( cmd == "R" ) the_pos[0] += 10.0;

		    if( cmd == "b" ) the_pos[1] -= 1.0;
		    if( cmd == "t" ) the_pos[1] += 1.0;
		    if( cmd == "B" ) the_pos[1] -= 10.0;
		    if( cmd == "T" ) the_pos[1] += 10.0;

		    if( cmd == "f" ) the_pos[1] = 10.0;
		    if( cmd == "m" ) the_pos[1] = 3.0;
		    if( cmd == "n" ) the_pos[1] = 1.5;
		    if( cmd == "F" ) the_pos[1] = 100.0;
		    if( cmd == "M" ) the_pos[1] = 30.0;
		    if( cmd == "N" ) the_pos[1] = 20;
		    if( cmd == "c" ) the_pos[1] = -2;
		    if( cmd == "nose" ) { the_pos[0] = -9.875; the_pos[1] = -0.3; }
		    if( cmd == "nose2" ) { the_pos[0] = -9.875; the_pos[1] = -0.2; }


		    cout << "sending movecommand" << the_pos[0] << " " << the_pos[1] << endl;
		    MoveCommand *mcmd = new MoveCommand( the_pos, source );
		    rteng->put( mcmd );
	    }
    }
    
    jackclient->deactivate();

    delete oscserv;
    delete rteng;
    delete spkarray;
    delete inputline;
    delete jackclient;
    return 0;
}
