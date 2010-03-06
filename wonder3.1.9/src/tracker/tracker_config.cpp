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

#include "tracker_config.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>

#include "config.h"
#include "wonder_path.h"

#include <getopt.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>


using std::cout;
using std::cerr;
using std::endl;
using std::list;
using std::string;
using std::stringstream;
using std::ifstream;

using xmlpp::Node;
using xmlpp::Element;
using xmlpp::Attribute;


TrackerConfig::TrackerConfig( int argc, char** argv )
{
    // set the default values
    listeningPort      = "58700";
    trackerConfigfile  = join( INSTALL_PREFIX, "share/wonder3/configs/tracker_config.xml" ); 
    verbose            = true;
    trackerType        = PTRACKER;
    trackerName        = "ptracker";
    omit               = 0;
    wait               = 1;

    latencyTestMode     = false;
    writeLatencyFile    = false;
    testPoints          = 5000;

    // use current time to build a file name for latency testing,
    // format is date_time_name, e.g. 190608_164500_name 
    int        charBufferLength = 80;
    time_t     rawtime;
    struct tm* timeinfo;
    char       buffer[ charBufferLength ];

    time( &rawtime );
    timeinfo = localtime( &rawtime );
    strftime( buffer, charBufferLength, "%d%m%y_%H%M%S_", timeinfo );

    latencyTestFileName = string( buffer ) + "tracker_latencytest_results.data";

    // Now parse the commandline args...
    parse_args( argc, argv );
}


TrackerConfig::~TrackerConfig()
{
    for( list< OSCClient >::iterator it = clients.begin(); it != clients.end(); ++it )
        it->releaseAddress();
}


void TrackerConfig::parse_args( int argc, char** argv )
{
    int c;

    while( 1 )
    {
        int option_index = 0;
        static struct option long_options[] =
        {
            { "configfile",    required_argument, 0, 'c' },
            { "listeningport", required_argument, 0, 'o' },
            { "verbose",       no_argument,       0, 'v' },
            { "omit",          required_argument, 0, 'm' },
            { "latencytest",   no_argument,       0, 'l' },
            { "testpoints",    required_argument, 0, 'p' },
            { "writetestfile", no_argument,       0, 'f' },
            { "testfile",      no_argument,       0, 'n' },
            { "slowdown",      required_argument, 0, 's' },
            { "help",          no_argument,       0, 'h' },
            { 0 }
        };

        c = getopt_long( argc, argv, "c:o:vm:lp:fn:s:h", long_options, &option_index );
        if( c == -1 )
            break;

        switch( c )
        {
            case 'c':
                trackerConfigfile = strdup( optarg );
                break;

            case 'o':
                listeningPort = strdup( optarg );
                break;

            case 'v':
                verbose = 1;
                break;

            case 'm':
                omit = atoi( optarg );
                if( omit < 0 )
                {
                    cerr << "invalid value for omit, setting it to 0 " << endl;
                    omit = 0 ;
                }
                break;

            case 'l':
                latencyTestMode = true;
                break;

            case 'p':
                testPoints = atoi( optarg );
                if( testPoints < 0 )
                {
                    cerr << "invalid value for test points, setting it to 1000" << endl;
                    testPoints = 1000 ;
                }
                break;

            case 'f':
                writeLatencyFile = true;
                break;

            case 'n':
                latencyTestFileName = strdup( optarg );
                break;

            case 's':
                wait = atoi( optarg );
                if( wait < 0 )
                {
                    cerr << "invalid value for slowing down tracker by uwait(arg), setting it to 1" << endl;
                    wait = 1 ;
                }
                break;

            case 'h':
                printf ("\ntracker's commandline arguments:\n"         
                        "--configfile,    -c ( path to the fwonder config file, which should be used )\n"
                        "--listeningport, -o ( port where tracker listens, default is 58700 )\n"
                        "--verbose,       -v ( print output to terminal )\n"
                        "--omit,          -m ( keep 1 message and omit next x messages )\n"
                        "--latencytest    -l ( do latency testing )\n"
                        "--testpoints     -p ( how many measurements the latency test should do )\n"
                        "--writetestfile  -f ( write results of latency test to file )\n"
                        "--testfile       -n ( where the latency test data should be saved )\n"
                        "--slowdown       -s ( slow down (i)tracker-app by uwait(arg), default is 1 )\n"
                        "--help,          -h \n\n");       
                exit( EXIT_SUCCESS );

            case '?':
                break;

            default:
                printf( "?? getopt returned character code 0%o ??\n", c );
        }
    }

    if ( optind < argc )
    {
        printf( "non-option ARGV-elements: " );
        while(optind < argc )
            printf( "%s ", argv[ optind++ ] );
        printf( "\n" );

        exit( EXIT_FAILURE );
    }
}


int TrackerConfig::readConfig()
{
    // check if file exist
    ifstream fin( trackerConfigfile.c_str(), std::ios_base::in );
    if( ! fin.is_open() )
    {
        return 1; // file does not exist
    }
    fin.close();

    // Read the Dom representation from the xml-file
    try
    {
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
        parser.parse_file( trackerConfigfile );
        if( parser )
        {
            xmlpp::Node* root = parser.get_document()->get_root_node();
            if( root ) 
            {        
                // validate the current dom representation
                // but first find the dtd
                string dtdPath;
                dtdPath = join( INSTALL_PREFIX, "share/wonder3/dtd/tracker_config.dtd");
                fin.open( dtdPath.c_str(), std::ios_base::in );
                if( ! fin.is_open() )
                    return 2; // dtd file does not exist
                         
                try
                {
                    xmlpp::DtdValidator validator( dtdPath.c_str() );
                    validator.validate( parser.get_document() );
                    //// std::cout << "[V-wonderconfig] Validation successfull" << std::endl << std::endl;
                }
                catch( const xmlpp::validity_error& ex)
                {
                    //// std::cout << "[V-wonderconfig] Error validating the document"<< std::endl 
                    //// << ex.what() << std::endl << std::endl;
                    return 3; /// dtd error
                }

                getTrackerSettings  ( root );
                getOSCClientSettings( root );
                

                if( trackerName.compare( "ptracker" ) == 0 )
                    trackerType = PTRACKER;
                else if( trackerName.compare( "itracker" ) == 0 )
                    trackerType = ITRACKER;
                else
                {
                    // this should never be reached because dtd validation should prevent
                    // usage of any invalid tracker types 
                    cerr << "Unknown trackertype, exiting..." << endl;
                    exit( EXIT_FAILURE );
                }
            }
        }
    }
    catch( const std::exception& ex )
    {
        //// cout << "Exception caught: " << ex.what() << endl;
        return 4; // xml error
    }   

    return 0;
}


void TrackerConfig::getTrackerSettings( Node* node )
{ 
    xmlpp::NodeSet nset = node->find( "/tracker_config/tracker" );

    if( nset.size() > 0 )
    {
        if( const Element* nodeElement = dynamic_cast< const Element* >( *( nset.begin() ) ) )
        {
            const Element::AttributeList& attributes = nodeElement->get_attributes();
            Element::AttributeList::const_iterator iter;
            for( iter = attributes.begin(); iter != attributes.end(); ++iter)
            {
                const Attribute* attribute = *iter;
                Glib::ustring nodename = attribute->get_name();
                stringstream ss( attribute->get_value() );
                if ( nodename == "type" )
                    ss >> trackerName; 
            }
        }
    }
}


void TrackerConfig::getOSCClientSettings( Node* node )
{    
    xmlpp::NodeSet nset = node->find( "/tracker_config/oscclient" );

    if( nset.size() > 0 )
    {
        for( xmlpp::NodeSet::iterator setIter = nset.begin(); setIter != nset.end(); ++setIter )
        {
            if( const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* >( *setIter ) ) 
            {
                OSCClient newClient;

                const Element::AttributeList& attributes = nodeElement->get_attributes();
                Element::AttributeList::const_iterator itera;
          
                for( itera = attributes.begin(); itera != attributes.end(); ++itera )
                {
                    const Attribute* attribute = *itera;
                    Glib::ustring attrname = attribute->get_name();
            
                    stringstream ss( attribute->get_value() );
                    if( attrname == "host" )
                        ss >> newClient.oscHost;
                    else if( attrname == "port" )
                        ss >> newClient.oscPort;
                    else if( attrname == "sendPan" )
                        ss >> newClient.sendPan; 
                    else if ( attrname == "sendTilt" )
                        ss >> newClient.sendTilt; 
                    else if ( attrname == "sendRot" )
                        ss >> newClient.sendRot; 
                }

                newClient.initAddress();
                clients.push_front( newClient );
            }
        }
    }
}


void TrackerConfig::print()
{
    cout << "Tracker configuration: " << trackerConfigfile << endl
         << "Tracker osc listening port = " << listeningPort << endl
         << "Tracker type = " << trackerName << endl;

    int clientNumber = 0;
    for( list< OSCClient >::const_iterator iter = clients.begin(); iter != clients.end(); ++iter )
    {
        ++clientNumber;
        cout << "OSCClient #" << clientNumber << endl
             << "\tHost = " << iter->oscHost << endl
             << "\tPort = " << iter->oscPort << endl
             << "\tsendPan  = " << ( iter->sendPan  ? "yes" : "no" )<< endl
             << "\tsendTilt = " << ( iter->sendTilt ? "yes" : "no" )<< endl
             << "\tsendRot  = " << ( iter->sendTilt ? "yes" : "no" )<< endl;
    }
}

TrackerConfig* trackerConf = NULL;
