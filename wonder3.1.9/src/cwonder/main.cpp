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

#include <iostream>
#include <fstream>
#include <sstream> 

#include <unistd.h>
#include <csignal>

#include "config.h"
#include "cwonder.h"
#include "oscinctrl.h"
#include "cwonder_config.h"
#include "wonder_path.h"

using namespace std;

std::string pidFilePath;

int start_daemon()
{
    pid_t pid;

    pid = fork();

    if( pid  < 0 )
	exit( EXIT_FAILURE );

    if( pid > 0 )
	exit( EXIT_SUCCESS );

    if( setsid() < 0 )
        exit( EXIT_FAILURE  );

    if( ( chdir( "/" ) ) < 0 ) 
        exit( EXIT_FAILURE  );

    umask( 0 );

    for( int i = sysconf( _SC_OPEN_MAX ); i > 0; i-- )
        close( i );


    // check if the directory for pid file exist
    ifstream fin;
    string path = "/tmp/wonder";
    fin.open( path.c_str() , ios_base::in );

    if( !fin.is_open() )
    {
        // create /tmp/wonder directory if it does not exist yet
        // the sticky: anyone is able to create files, but
        // not able to delete anyone elses files but their own
        int error = mkdir( path.c_str(), mode_t( S_IRWXU | S_IRWXG | S_IRWXO | S_ISVTX ) );
        if( error != 0 )
            return 1;   
    }

    // open file stream to write in the pid 
    string        name;
    ostringstream pidString;

    // get pid of parent process
    pidString << getpid();
    if( cwonderConf->user == NULL )
        pidFilePath = path + "/cwonder." + pidString.str() + ".pid";
    else
        pidFilePath = path + "/cwonder." + ( string ) cwonderConf->user + ".pid";
    ofstream pidFile( pidFilePath.c_str() );
    pidFile << pidString.str();
    pidFile.close();

    // write stdin to /dev/null
    freopen( "/dev/null", "r", stdin );

    // write stdout and stderr to the log file     
    freopen( "/var/log/wonder.log", "w", stdout );
    freopen( "/var/log/wonder.log", "w", stderr );

    return 0;
}


int cleanDaemon()
{
    // delete the pid file
    if( cwonderConf->daemon )
    {   
        if( remove( pidFilePath.c_str() ) != 0 )
            wonderlog->print( LOG_ERR, "[E-Config] Error deleting pid file." );          
            return 1;   
    }    
    wonderlog->print( LOG_INFO, "[V-Config] Deleted pid file." );        

    return 0;
}


void signalHandler( int sig )
{
    cleanDaemon();        
    wonderlog->print( LOG_ERR, "[E-Wonder] signal received, exiting ..." );
    exit( 0 );
}


int main( int argc, char** argv ) 
{
    cwonderConf = new CwonderConfig( argc, argv );

    // create log 
    wonderlog = new WonderLog( "cwonder" );
    if( cwonderConf->verbose )
        wonderlog->setMode( 2 ); // print all messages
    else
        wonderlog->setMode( 0 ); // print only error messages

    wonderlog->print( LOG_INFO, "[V] ----------start cwonder--------" );
    wonderlog->print( LOG_INFO, "[V-settings] config path=" + cwonderConf->cwonderConfigFile );


    int ret = cwonderConf->readConfig();
    if( ret != 0 )
    {
        if( ret == 1 )
            wonderlog->print( LOG_ERR, "[E-cwonderConfig] cwonderConfig file does not exist." );
        else if( ret == 2 )
            wonderlog->print( LOG_ERR, (string)"[E-cwonderConfig] dtd file does not exist.");
        else if( ret == 3 )
            wonderlog->print( LOG_ERR, "[E-cwonderConfig] dtd error. Check if the xml file got the correct dtd." );
        else
            wonderlog->print( LOG_ERR, "[E-cwonderConfig] xml error. Check if the xml file is correct." );
        exit( EXIT_FAILURE );
    }

    // make default project path
    if( makedirs( cwonderConf->projectPath.c_str(), mode_t(0700) ) != 0 )
        wonderlog->print( LOG_ERR, "[E-settings] error making default project path." );

    // create the control application 
    Cwonder* cwonder     = new Cwonder();
    cwonder->dtdPath     = join( INSTALL_PREFIX, "share/wonder3/dtd" ); 
    cwonder->projectPath = cwonderConf->projectPath;

    wonderlog->print( LOG_INFO, "[V-settings] dtd path=" + ( string ) cwonder->dtdPath );
    wonderlog->print( LOG_INFO, "[V-settings] project path=" + ( string ) cwonder->projectPath );

    // daemonize
    if( cwonderConf->daemon )
    {
        if( start_daemon() != 0 )
        {
            wonderlog->print( LOG_ERR, "[E-Config] mkdir /tmp/wonder directory error.");
            cleanDaemon();
            return 1;
        }
    }

    OSCControl* oscctrl;

    try
    {
        oscctrl = new OSCControl( cwonderConf->listeningPort );    
    }
    catch( OSCServer::EServ )
    {
        wonderlog->print( LOG_ERR, ( string ) "[E-OSCServer] Could not create server," + 
                                   ( string ) "maybe the server( using the same port ) is already running?" );                   
        cerr << "[E-OSCServer] Could not create server, maybe the server( using the same port ) is already running?" << endl;                   
        cleanDaemon();
        return 0;
    }

    // add the functions to the osc server 
    oscctrl->addReply( cwonder );
    oscctrl->addMethods();

    // start the OSC receive thread
    oscctrl->start();

    // add signal handlers
    signal( SIGQUIT, signalHandler );
    signal( SIGTERM, signalHandler );
    signal( SIGHUP,  signalHandler );
    signal( SIGINT,  signalHandler );  

    // now wait for incoming OSC messages
    while( 1 )
    {
        usleep( 1000 );         
    }


    // delete the pid file
    cleanDaemon();

    delete cwonder;
    delete oscctrl;
    delete cwonderConf;
    delete wonderlog;

    return EXIT_SUCCESS;    
}
