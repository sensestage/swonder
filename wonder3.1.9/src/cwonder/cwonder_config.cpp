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

#include "cwonder_config.h"

#include "wonder_path.h"
#include "config.h"
#include <libxml++/libxml++.h>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

using namespace std;


CwonderConfig::CwonderConfig( int argc, char** argv ) 
{
    verbose    = false;
    oscverbose = false;
    daemon     = false;
    basicMode  = false;

    cwonderConfigFile = join( INSTALL_PREFIX, "share/wonder3/configs/cwonder_config.xml" ); 

    user          = NULL;
    pingRate      = 44100;
    listeningPort = "58100";
    projectPath   = "";
    roomName      = "";

    parseArgs( argc, argv );
}


void CwonderConfig::parseArgs( int argc, char **argv ) 
{
    int c;
    int option_index = 0;

    static struct option long_options[] =
    {
        { "daemon",        no_argument,        0, 'd' },
        { "configfile",    required_argument,  0, 'c' },
        { "user",          required_argument,  0, 'u' },
        { "listeningport", required_argument,  0, 'o' },
        { "pingrate",      required_argument,  0, 'r' },
        { "basicmode",     no_argument,        0, 'b' },
        { "verbose",       no_argument,        0, 'v' },
        { "oscverbose",    no_argument,        0, 's' },
        { "help",          no_argument,        0, 'h' },
        { 0, 0, 0, 0 }
    };

    while( 1 )
    {
        c = getopt_long (argc, argv, "dc:u:o:r:bvsh", long_options, &option_index);
        
        if( c == -1 )
            break;
        
        switch ( c )
        {         
            case 'd':
                daemon = true;
                break;

            case 'c':
                cwonderConfigFile = strdup( optarg );
                break;

            case 'u':
                user = strdup( optarg );
                break;

            case 'o':
                listeningPort = strdup( optarg );
                break;

            case 'r':
                {
                    int temp = atoi( optarg );
                    if( temp > 0 )
                        pingRate = temp;
                }
                break;

            case 'b':
                basicMode = true;
                break;

            case 'v':
                verbose = true;
                break;

            case 's':
                oscverbose = true;
                break;
                
            case 'h':
                printf ("\ncwonder's commandline arguments:\n"         
                        "--daemon,        -d\n"
                        "--configfile,    -c ( path to cwonder's config file )\n"
                        "--user,          -u ( user starting the process; part of name of pidfile; else the pid is used )\n"
                        "--listeningport, -o ( port where cwonder listens, default is 58100 )\n"
                        "--pingrate,      -r ( rate at which the pings are send to the streamlisteners (in samples) )\n"
                        "--basicmode,     -b ( basicmode, no project functionality, just dispatching OSC messages, all sources active )\n"
                        "--verbose,       -v ( print output to terminal )\n"
                        "--oscverbose,    -s ( print osc communication to terminal )\n"
                        "--help,          -h \n\n");       
                exit( EXIT_SUCCESS );
                
            case '?':
                break;
         
            default:
                printf ("?? getopt returned character code 0%o ??\n", c);

        }
    }
    
    // Print any remaining command line arguments (not options).
    if( optind < argc )
    {
        printf( "non-option ARGV-elements: " );
        while( optind < argc )
            printf( "%s ", argv[ optind++ ] );
        printf( "\n" );
        exit( EXIT_FAILURE );
    }
}


int CwonderConfig::readConfig()
{
    // check if file exist
    ifstream fin( cwonderConfigFile.c_str(), ios_base::in );

    if( ! fin.is_open() )
    {
        // cout << endl <<"[E-wonderconfig] file=" << filepath << " does not exist." << endl;
        return 1; // file does not exist
    }
    fin.close();

    // Read the Dom representation from a xml-file
    try
    {
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
        parser.parse_file( cwonderConfigFile );
        if( parser )
        {
            xmlpp::Node* root = parser.get_document()->get_root_node();
            if( root ) 
            {        
                // validate the current dom representation
                // but first find the dtd
                string dtdPath;
                dtdPath = join( INSTALL_PREFIX, "share/wonder3/dtd/cwonder_config.dtd");
                fin.open( dtdPath.c_str(), ios_base::in );
                if( ! fin.is_open() )
                {
                    // std::cout << "[E-wonderconfig] dtd file does not exist." << std::endl;
                    return 2; // dtd file does not exist
                }
                         
                try
                {
                    xmlpp::DtdValidator validator( dtdPath.c_str() );
                    validator.validate( parser.get_document() );
                    // std::cout << "[V-wonderconfig] Validation successfull" << std::endl << std::endl;
                }
                catch( const xmlpp::validity_error& ex )
                {
                    // std::cout << "[V-wonderconfig] Error validating the document"<< std::endl 
                    // << ex.what() << std::endl << std::endl;
                    return 3; // dtd error
                }

                getSettings( root );
                getRenderPolygon( root );
            }
        }
    }
    catch( const exception& ex )
    {
        // cout << "Exception caught: " << ex.what() << endl;
        return 4; // xml error
    }   

    return 0;
}


void CwonderConfig::getSettings( xmlpp::Node* node )
{    
    xmlpp::NodeSet nset = node->find( "/cwonder_config/settings" );

    if( nset.size() > 0 )
    {
        if( const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* >( *nset.begin() ) ) 
        {
            const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
            xmlpp::Element::AttributeList::const_iterator iter;

            for(iter = attributes.begin(); iter != attributes.end(); ++iter)
            {
                const xmlpp::Attribute* attribute = *iter;
                
                Glib::ustring nodeName = attribute->get_name();
            
                stringstream ss( attribute->get_value() );
                if ( nodeName == "projectpath" )
                    ss >> projectPath; 
                else if ( nodeName == "maxNoSources" )
                    ss >> maxNoSources; 
                                
            }                   
        }
    }
}


void CwonderConfig::getRenderPolygon( xmlpp::Node* node )
{
    xmlpp::NodeSet nset = node->find( "/cwonder_config/renderpolygon" );

    if( nset.size() > 0 )
    {
        if( const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* >( *nset.begin() ) ) 
            roomName = nodeElement->get_attribute("roomname")->get_value();

        xmlpp::NodeSet pointsSet = node->find( "/cwonder_config/renderpolygon/point" );

        // iterate over all points and add them to vector roomPoints
        xmlpp::NodeSet::iterator pointIt;
        for( pointIt = pointsSet.begin(); pointIt != pointsSet.end(); ++pointIt)
        {
            const xmlpp::Element* pointElement = dynamic_cast< const xmlpp::Element* >( *pointIt );  

            Vector3D point;
            float x;
            float y;
            float z;

            istringstream sx( pointElement->get_attribute( "x" )->get_value() );
            sx >> x;
            istringstream sy( pointElement->get_attribute( "y" )->get_value() );
            sy >> y;
            istringstream sz( pointElement->get_attribute( "z" )->get_value() );
            sz >> z;
            point.set( x, y, z );
            renderPolygonPoints.push_back( point );
        }                   
    }
}

CwonderConfig* cwonderConf = NULL;
WonderLog*     wonderlog   = NULL;
