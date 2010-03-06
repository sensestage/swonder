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

#include "twonder_config.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <getopt.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "config.h"
#include "wonder_path.h"

#include <libxml++/libxml++.h>


TwonderConfig::TwonderConfig( int argc, char** argv )
{
    daemon     = false;
    verbose    = false;
    nonRTDebug = false;

    twonderConfigfile  = join( INSTALL_PREFIX, "share/wonder3/configs/twonder_config.xml" ); 
    speakersConfigfile = join( INSTALL_PREFIX, "share/wonder3/configs/twonder_speakerarray.xml" );

    jackName  = "twonder";
    name      = "twonder";
    planeComp = 0.2; // XXX:empirical value, might need tweaking

    noSources   = 0;

    cwonderHost   = "127.0.0.1";
    cwonderPort   = "58100";

    listeningPort = "58200";
    user          = NULL;

    sampleRate   = 44100;
    soundSpeed   = 340.0;
    negDelayInit = 20.0;
    ioMode       = IOM_NORMAL;

    reference       = 0.0;
    focusLimit      = 0.0;
    focusMargin     = 0.0;
    speakerDistance = 0.0;

    elevationY1 = 0.0;
    elevationY2 = 0.0;
    elevationZ1 = 0.0;
    elevationZ2 = 0.0;
    slope       = 0.0;

    parseArgs( argc, argv );

    // get cwonders osc address
    cwonderAddr = lo_address_new( cwonderHost, cwonderPort );
}


TwonderConfig::~TwonderConfig()
{
    if( cwonderAddr )
    {
        lo_address_free( cwonderAddr );
        cwonderAddr = NULL;
    }
}


void TwonderConfig::parseArgs( int argc, char** argv )
{
    int c;

    while( 1 )
    {
        int option_index = 0;
        static struct option long_options[] =
        {
            {"daemon",        no_argument,       0, 'd' },
            {"configfile",    required_argument, 0, 'c' },
            {"user",          required_argument, 0, 'u' },
            {"speakerfile",   required_argument, 0, 's' },
            {"jackname",      required_argument, 0, 'j' },
            {"name",          required_argument, 0, 'n' },
            {"listeningport", required_argument, 0, 'o' },
            {"cwonderhost",   required_argument, 0, 'i'},
            {"cwonderport",   required_argument, 0, 'p'},
            {"planecomp",     required_argument, 0, 'm' },
            {"verbose",       no_argument,       0, 'v' },
            {"negdelay",      required_argument, 0, 1   },
            {"speedofsound",  required_argument, 0, 2   },
            {"alwaysout",     no_argument,       0, 3   },
            {"alwaysin",      no_argument,       0, 4   },
            {"nonrtdebug",    no_argument,       0, 5   },
            {"help",          no_argument,       0, 'h' },
            { 0 }
        };

        c = getopt_long( argc, argv, "dc:u:s:j:n:o:i:p:m:vh", long_options, &option_index );
        if( c == -1 )
            break;

        switch( c )
        {
            case 'd':
                daemon = 1;
                break;

            case 'c':
                twonderConfigfile = strdup( optarg );
                break;

            case 'u':
                user = strdup( optarg );
                break;

            case 's':
                speakersConfigfile = strdup( optarg );
                break;

            case 'j':
                jackName = strdup( optarg );
                break;
                
            case 'n':
                name = strdup( optarg );
                break;

            case 'o':
                listeningPort = strdup( optarg );
                break;

            case 'i':
                cwonderHost = strdup(optarg);
                break;

            case 'p':
                cwonderPort = strdup(optarg);
                break;

            case 'm':
                {
                    float temp = atof( optarg );
                    if( temp >= 0.0  &&  temp <= 1.0 )
                        planeComp = temp;
                    else
                        std::cerr << "Invalid value for argument --planecomp ( -m ), must be [ 0.0, 1.0 ]." << std::endl;
                }
                break;

            case 'v':
                verbose = 1;
                break;

            case 1:
                soundSpeed = atof( optarg );
                break;

            case 2:
                negDelayInit = atof( optarg );
                break;

            case 3:
                ioMode = IOM_ALWAYSOUT;
                break;

            case 4:
                ioMode = IOM_ALWAYSIN;
                break;

            case 5:
                nonRTDebug = true;
                break;

            case 'h':
                printf ("\ntwonder's commandline arguments:\n"         
                        "--daemon,        -d\n"
                        "--configfile,    -c ( path to twonder's config file )\n"
                        "--speakerfile,   -s ( path to twonder's speaker file )\n"
                        "--user,          -u ( user starting the process; part of name of pidfile; else the pid is used )\n"
                        "--jackname,      -j ( name with which twonder registers with jack )\n"
                        "--name,          -n ( name with which twonder registers with cwonder )\n"
                        "--listeningport, -o ( port where twonder listens, default is 58200 )\n"
                        "--cwonderhost,   -i ( ip-address where cwonder is running )\n"
                        "--cwonderport,   -p ( port where cwonder can be reached )\n"
                        "--planecomp,     -m ( factor to compensate for the fact that planewaves are louder than point sources )\n"
                        "--verbose,       -v ( print output to terminal )\n"
                        "--negdelay          ( initital maximum negative delay in meters )\n"
                        "--speedofsound      ( in meters per second )\n"
                        "--alwaysin          ( sound is always rendered as focused source )\n"
                        "--alwaysout         ( sound is always rendered as not focused source )\n"
                        "--nonrtdebug        ( prints data just for testing purposes, reduces performance )\n"
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
        while( optind < argc )
            printf( "%s ", argv[ optind++ ] );
        printf( "\n" );

        exit( EXIT_FAILURE);
    }
}


int TwonderConfig::readConfig()
{
    // check if file exist
    std::ifstream fin( twonderConfigfile.c_str(), std::ios_base::in );
    if( ! fin.is_open() )
        return 1; // file does not exist

    fin.close();

    // Read the Dom representation from the xml-file
    try
    {
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
        parser.parse_file( twonderConfigfile );
        if( parser )
        {
            xmlpp::Node* root = parser.get_document()->get_root_node();
            if( root ) 
            {        
                // validate the current dom representation
                // but first find the dtd
                std::string dtdPath;
                dtdPath = join( INSTALL_PREFIX, "share/wonder3/dtd/twonder_config.dtd");
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

                getFocus   ( root );
                getSpeakers( root );
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


void TwonderConfig::getFocus( xmlpp::Node* node )
{    
    xmlpp::NodeSet nset = node->find( "/twonder_config/focus" );

    if( nset.size() > 0 )
    {
        if( const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>( *nset.begin() ) ) 
        {
            const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
            xmlpp::Element::AttributeList::const_iterator iter;

            for(iter = attributes.begin(); iter != attributes.end(); ++iter)
            {
                const xmlpp::Attribute* attribute = *iter;
                
                Glib::ustring nodeName = attribute->get_name();
            
                std::stringstream ss( attribute->get_value() );
                if ( nodeName == "limit" )
                    ss >> focusLimit; 
                else if ( nodeName == "margin" )
                    ss >> focusMargin; 
                                
            }                   
        }
    }
}


void TwonderConfig::getSpeakers( xmlpp::Node *node )
{    
    xmlpp::NodeSet nset = node->find( "/twonder_config/speakers" );

    if( nset.size() > 0 )
    {
        if( const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>( *nset.begin() ) ) 
        {
            const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
            xmlpp::Element::AttributeList::const_iterator iter;

            for(iter = attributes.begin(); iter != attributes.end(); ++iter)
            {
                const xmlpp::Attribute* attribute = *iter;
                
                Glib::ustring nodeName = attribute->get_name();
            
                std::stringstream ss( attribute->get_value() );
                if ( nodeName == "reference" )
                    ss >> reference; 
                else if ( nodeName == "distance" )
                    ss >> speakerDistance; 
            }                   
        }
    }
}

TwonderConfig* twonderConf = NULL;
