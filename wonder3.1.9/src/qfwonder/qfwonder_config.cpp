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
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include "qfwonder_config.h"
#include "config.h"
#include "wonder_path.h"


QFwonderConfig::QFwonderConfig( int argc, char** argv )
{
    // set default values
    qfwonderConfigfile  = join( INSTALL_PREFIX, "share/wonder3/configs/fwonder_config.xml" ); 
    listeningPort = "58400";

    parseArgs( argc, argv );
}


QFwonderConfig::~QFwonderConfig()
{
}


void QFwonderConfig::parseArgs( int argc, char**argv )
{
    int c;

    while( true )
    {
	int option_index = 0;
	static struct option long_options[] = 
        {
	    { "configfile",     required_argument, NULL, 'c' },
	    { "listeningport",  required_argument, NULL, 'o' },
	    { "help", no_argument,                 NULL, 'h' },
	    { 0, 0, 0, 0 }
	};

	c = getopt_long ( argc, argv, "c:o:h", long_options, &option_index );
	if( c == -1 )
	    break;

	switch( c )
        {
	    case 'c':
	        qfwonderConfigfile = strdup( optarg );  
	        break;
	    case 'o':
		listeningPort = strdup( optarg );
		break;
		break;
            case 'h':
                printf ("\nqfwonder's commandline arguments:\n"         
                        "--configfile,     -c ( path to qfwonder's config file, use the file you used when starting fwonder )\n"
                        "--listeningport,  -o ( port where qfwonder listens, default is 58400 )\n"
                        "--help,           -h \n\n");       
                exit( EXIT_SUCCESS );
	    case '?':
		break;
	    default:
		printf( "?? getopt returned character code 0%o ??\n", c );
	}
    }

    if( optind < argc ) 
    {
	printf( "non-option ARGV-elements: " );
	while( optind < argc )
            printf ("%s ", argv[ optind++ ] );
	printf ("\n");

	exit( EXIT_FAILURE );
    }
}


int QFwonderConfig::readConfig()
{
    // check if file exist
    std::ifstream fin( qfwonderConfigfile.c_str(), std::ios_base::in );
    if( ! fin.is_open() ) 
        return 1; // file does not exist

    fin.close();

    // Read the Dom representation from a xml-file
    try
    {
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
	parser.parse_file( qfwonderConfigfile );
	if( parser )
        {
            xmlpp::Node* root = parser.get_document()->get_root_node();
	    if( root )
            {        
                // validate the current dom representation
                // but first find the dtd
                std::string dtdPath;
                dtdPath = join( INSTALL_PREFIX, "share/wonder3/dtd/fwonder_config.dtd");
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
                    return 3; /// dtd error
                }

		getBRIRSettings( root );
            }
        }
    }
    catch( const std::exception& ex )
    {
        return 4; // xml error
    }   

    return 0;
}


void QFwonderConfig::getBRIRSettings( xmlpp::Node* node )
{    
    xmlpp::Node::NodeList list = node->get_children();
    for( xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter )
    {
        const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* >( *iter );
        if( nodeElement )
        {
            Glib::ustring nodename = nodeElement->get_name();
            if( nodename == "brir" )
            {
                xmlpp::Node::NodeList child_list = nodeElement->get_children();
                for( xmlpp::Node::NodeList::iterator iter_child = child_list.begin(); iter_child != child_list.end(); ++iter_child )
                {
                    const xmlpp::Element* childNodeElement = dynamic_cast<const xmlpp::Element*>(*iter_child);
                    if ( childNodeElement )
                    {
                        Glib::ustring childnodename = childNodeElement->get_name();
                        if( childnodename == "azimuth" )
                        {
                            const xmlpp::Element::AttributeList& attributes = childNodeElement->get_attributes();
                            xmlpp::Element::AttributeList::const_iterator itera;

                            for( itera = attributes.begin(); itera != attributes.end(); ++itera )
                            {
                                const xmlpp::Attribute* attribute = *itera;
                                Glib::ustring attrname= attribute->get_name();

                                std::stringstream ss( attribute->get_value() );
                                if( attrname == "start" )
                                    ss >> sourceAzimuthStart; 
                                else if( attrname == "stop" )
                                    ss >> sourceAzimuthStop; 
                            }
                        }	 
                        else if( childnodename == "elevation" )
                        {
                            const xmlpp::Element::AttributeList& attributes = childNodeElement->get_attributes();
                            xmlpp::Element::AttributeList::const_iterator itera;

                            for( itera = attributes.begin(); itera != attributes.end(); ++itera )
                            {
                                const xmlpp::Attribute* attribute = *itera;
                                Glib::ustring attrname = attribute->get_name();

                                std::stringstream ss( attribute->get_value() );
                                if( attrname == "start" )
                                    ss >> sourceElevationStart; 
                                else if( attrname == "stop" )
                                    ss >> sourceElevationStop; 
                            }
                        }
                    }
                }
            }
        }
    }
}

QFwonderConfig* qfwonderConf = NULL;
