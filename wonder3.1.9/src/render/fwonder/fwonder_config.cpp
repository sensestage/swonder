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

#include "fwonder_config.h"
#include "config.h"
#include "wonder_path.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <getopt.h>
#include <unistd.h>

#include <libxml++/libxml++.h>


FwonderConfig::FwonderConfig( int argc, char** argv )
{
    //set default values for commandline arguments
    sampleRate    = 0;
    listeningPort = "58500";

    qfwonderHost   = "127.0.0.1";
    qfwonderPort   = "58400";

    IRVerbose            = false;
    oscResolutionVerbose = false;
    oscMoveVerbose       = false;

    fwonderConfigfile  = join( INSTALL_PREFIX, "share/wonder3/configs/fwonder_config.xml" ); 

    jackName  = "fwonder";
    noSources = 0;
    noOutputs = 0;

    doCrossfades  = false;
    useTail       = false;
    maxIRLength   = 0;

    //Tail Settings
    doTailCrossfades  = false;
    tailPartitionSize = 1;
    maxTailLength     = 0;
    tailOffset        = 0;
    tailWindow        = LINEAR; 
    tailName          = "";

    //Dynamic Cache Settings
    dynamicCacheXResolution = 1;
    dynamicCacheYResolution = 1;
    dynamicCacheXRadius     = 0;
    dynamicCacheYRadius     = 0;

    //Static Cache Settings
    staticCacheXResolution = 1;
    staticCacheYResolution = 1;
    
    // Advanced Settings
    resolutionChangeable = false;

    //BRIR Settings
    BRIRPath = "";
    sourceAzimuthStart   = 0;
    sourceAzimuthStop    = 0;
    sourceElevationStart = 0;
    sourceElevationStop  = 0;

    // Now parse the commandline args...
    parseArgs( argc, argv );

    // get qfwonders osc address
    qfwonderAddr = lo_address_new( qfwonderHost, qfwonderPort );
}


FwonderConfig::~FwonderConfig()
{
    if( qfwonderAddr )
    {
        lo_address_free( qfwonderAddr );
        qfwonderAddr = NULL;
    }
}


void FwonderConfig::parseArgs( int argc, char**argv )
{
    int c;

    while( true )
    {
	int option_index = 0;
	static struct option long_options[] = 
        {
	    { "configfile",           required_argument, NULL, 'c' },
	    { "listeningport",        required_argument, NULL, 'o' },
            { "qfwonderhost",         required_argument, NULL, 'i'},
            { "qfwonderport",         required_argument, NULL, 'p'},
	    { "irverbose",            no_argument,       NULL, 'v' },
	    { "oscmoveverbose",       no_argument,       NULL, 'm' },
	    { "oscresolutionverbose", no_argument,       NULL, 'r' },
	    { "help", no_argument,                 NULL, 'h' },
	    { 0, 0, 0, 0 }
	};

	c = getopt_long ( argc, argv, "c:o:vmrh", long_options, &option_index );
	if( c == -1 )
	    break;

	switch( c )
        {
	    case 'c':
	        fwonderConfigfile = strdup( optarg );  
	        break;
	    case 'o':
		listeningPort = strdup( optarg );
		break;
            case 'i':
                qfwonderHost = strdup( optarg );
                break;
            case 'p':
                qfwonderPort = strdup( optarg );
                break;
	    case 'v':
		IRVerbose = 1;
		break;
	    case 'r':
		oscResolutionVerbose = 1;
		break;
	    case 'm':
		oscMoveVerbose = 1;
		break;
            case 'h':
                printf ("\nfwonder's commandline arguments:\n"         
                        "--configfile,           -c ( path to fwonder's config file )\n"
                        "--listeningport,        -o ( port where fwonder listens, default is 58500 )\n"
                        "--qfwonderhost,         -i ( ip-address where qfwonder is running )\n"
                        "--qfwonderport,         -p ( port where qfwonder can be reached )\n"
                        "--irVerbose,            -v ( print information about impulse responses to terminal )\n"
                        "--oscresolutionverbose, -r ( print received osc grid messages to terminal )\n"
                        "--oscmoveverbose,       -m ( print received osc move messages to terminal )\n"
                        "--help,                 -h \n\n");       
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


int FwonderConfig::readConfig()
{
    // check if file exist
    std::ifstream fin( fwonderConfigfile.c_str(), std::ios_base::in );
    if( ! fin.is_open() ) 
        return 1; // file does not exist

    fin.close();

    // Read the Dom representation from a xml-file
    try
    {
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
	parser.parse_file( fwonderConfigfile );
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
                    //// << ex.what() << std::endl << std::endl;
                    return 3; /// dtd error
                }

                try
                {
                    getJackSettings    ( root );
                    getBRIRSettings    ( root );
                    getIRSettings      ( root );
                    getTailSettings    ( root );
                    getAdvancedSettings( root );
                }
                catch( ... )
                {
                    // configfile contains invalid settings
                    return 5;
                }
            }
        }
    }
    catch( const std::exception& ex )
    {
       // cerr << "Exception caught: " << ex.what() << std::endl;
        return 4; // xml error
    }   

    return 0;
}


void FwonderConfig::getJackSettings( xmlpp::Node* node )
{    
    xmlpp::Node::NodeList list = node->get_children();
    for( xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* > ( *iter );
	if( nodeElement )
        {
            Glib::ustring nodename = nodeElement->get_name();
            if( nodename == "jack" )
            {
                const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
                xmlpp::Element::AttributeList::const_iterator itera;
	       
                for( itera = attributes.begin(); itera != attributes.end(); ++itera)
                {
                    const xmlpp::Attribute* attribute = *itera;
                    Glib::ustring attrname= attribute->get_name();
		
                    std::stringstream ss( attribute->get_value() );
                    if( attrname == "number_of_sources" )
                        ss >> noSources; 
                    else if( attrname == "number_of_outputs" )
                        ss >> noOutputs;
                    else if( attrname == "name" )
                        ss >> jackName; 
                }
            }
	}
    }

    // check for invalid settings
    if( noOutputs < 2 )
    {
        std::cerr << "Fwonder needs at least 2 outputs, please check your configuration file." << std::endl;
        throw 0;
    }
}


void FwonderConfig::getBRIRSettings( xmlpp::Node* node )
{    
    xmlpp::Node::NodeList list = node->get_children();
    for( xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter )
    {
        const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* > ( *iter );
        if( nodeElement )
        {
            Glib::ustring nodename = nodeElement->get_name();
            if( nodename == "brir" )
            {
                const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
                xmlpp::Element::AttributeList::const_iterator itera;
	  
	        for( itera = attributes.begin(); itera != attributes.end(); ++itera )
                {
                    const xmlpp::Attribute* attribute = *itera;
                    Glib::ustring attrname= attribute->get_name();
	    
                    std::stringstream ss( attribute->get_value() );
                    if( attrname == "path" )
                        ss >> BRIRPath; 
                }

                xmlpp::Node::NodeList child_list = nodeElement->get_children();
                for( xmlpp::Node::NodeList::iterator iter_child = child_list.begin(); iter_child != child_list.end(); ++iter_child )
                {
                    const xmlpp::Element* childNodeElement = dynamic_cast< const xmlpp::Element* > ( *iter_child );
                    if( childNodeElement )
                    {
                        Glib::ustring childnodename = childNodeElement->get_name();
                        if( childnodename == "azimuth" )
                        {
                            const xmlpp::Element::AttributeList& attributes = childNodeElement->get_attributes();
                            xmlpp::Element::AttributeList::const_iterator itera;
		  
                            for( itera = attributes.begin(); itera != attributes.end(); ++itera )
                            {
                                const xmlpp::Attribute* attribute = *itera;
                                Glib::ustring attrname = attribute->get_name();
		    
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
    // check for invalid settings
    if( sourceAzimuthStart > sourceAzimuthStop  ||  sourceElevationStart > sourceElevationStop )
    {
        std::cerr << "Azimuth and elevation starting values must not be smaller than stop values!" << std::endl;
        throw 0;
    }
}


void FwonderConfig::getIRSettings( xmlpp::Node* node )
{    
    xmlpp::Node::NodeList list = node->get_children();
    for( xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter )
    {
        const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* > ( *iter );
        if( nodeElement )
        {
            Glib::ustring nodename = nodeElement->get_name();
            if( nodename == "impulseresponse" )
            {
                const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
                xmlpp::Element::AttributeList::const_iterator itera;
	  
                for( itera = attributes.begin(); itera != attributes.end(); ++itera)
                {
                    const xmlpp::Attribute* attribute = *itera;
                    Glib::ustring attrname = attribute->get_name();
	    
                    std::stringstream ss( attribute->get_value() );
                    if( attrname == "do_crossfades" )
                        ss >> doCrossfades; 
                    else if( attrname == "max_length" )
                        ss >> maxIRLength;
                }
            }
            else if( nodename == "static_ir_matrix" )
            {
                const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
                xmlpp::Element::AttributeList::const_iterator itera;

	        for( itera = attributes.begin(); itera != attributes.end(); ++itera ) 
                {
                    const xmlpp::Attribute* attribute = *itera;
                    Glib::ustring attrname = attribute->get_name();
	    
                    std::stringstream ss( attribute->get_value() );
                    if( attrname == "x_resolution" )
                        ss >> staticCacheXResolution; 
                    else if( attrname == "y_resolution" )
                        ss >> staticCacheYResolution;
                }		
            }
            else if( nodename == "dynamic_ir_matrix" )
            {
                const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
                xmlpp::Element::AttributeList::const_iterator itera;
	  
                for( itera = attributes.begin(); itera != attributes.end(); ++itera )
                {
                    const xmlpp::Attribute* attribute = *itera;
                    Glib::ustring attrname = attribute->get_name();
                    std::stringstream ss( attribute->get_value() );
                    if( attrname == "x_resolution" )
                        ss >> dynamicCacheXResolution;
                    else if( attrname == "y_resolution" )
	                ss >> dynamicCacheYResolution;
	            else if( attrname == "x_radius" )
	                ss >> dynamicCacheXRadius;
	            else if( attrname == "y_radius" )
	                ss >> dynamicCacheYRadius;
                }
            }
        }
    }

    // check for invalid settings
    if( staticCacheXResolution  <= 0 || staticCacheYResolution  <= 0 ||
        dynamicCacheXResolution <= 0 || dynamicCacheYResolution <= 0    )
    {
        std::cerr << "Resolution values must not be smaller than 1!" << std::endl;
        throw 0;
    }
    if(  dynamicCacheXRadius <  0 || dynamicCacheYRadius <  0  )
    {
        std::cerr << "Radius values must not be smaller than 0!" << std::endl;
        throw 0;
    }
    // calculate maximum number of dynamically loaded IRs

}


void FwonderConfig::getTailSettings( xmlpp::Node* node )
{  
    xmlpp::Node::NodeList list = node->get_children();
    for( xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter )
    {
        const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(*iter);
        if( nodeElement )
        {
            Glib::ustring nodename = nodeElement->get_name();
            if( nodename == "tail" )
            {
                useTail = true;

                const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
                xmlpp::Element::AttributeList::const_iterator itera;
	  
	        for(itera = attributes.begin(); itera != attributes.end(); ++itera)
                {
                    const xmlpp::Attribute* attribute = *itera;
                    Glib::ustring attrname = attribute->get_name();

                    std::stringstream ss( attribute->get_value() );	
                    if( attrname == "name" )
                        ss >> tailName; 
                    else if( attrname == "max_length" )
                    {
                        ss >> maxTailLength; 
                        if( maxTailLength == 0 )
                            useTail = false;
                    }
                    else if( attrname == "partition_size" )
                        ss >> tailPartitionSize; 
                    else if( attrname == "window" )
                    {
                        std::string tempString;
                        ss >> tempString;
                        if( tempString.compare( "LINEAR" ) == 0 )
                            tailWindow = LINEAR; 
                        else if( tempString.compare( "COS2" ) == 0 )
                            tailWindow = COS2; 
                        else if( tempString.compare( "NOWIN" ) == 0 )
                            tailWindow = NOWIN; 
                    }
                    else if( attrname == "do_crossfades" )
                      ss >> doTailCrossfades; 
                    else if( attrname == "offset" )
                      ss >> tailOffset; 
                }
            }
        }
    }

    // check for invalid settings
    if( tailPartitionSize <= 0 )
    {
        std::cerr << "Tail partition size must not be smaller than 1!" << std::endl;
        throw 0;
    }
}


void FwonderConfig::getAdvancedSettings( xmlpp::Node* node )
{
    xmlpp::Node::NodeList list = node->get_children();
    for( xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        const xmlpp::Element* nodeElement = dynamic_cast< const xmlpp::Element* > ( *iter );
	if( nodeElement )
        {
            Glib::ustring nodename = nodeElement->get_name();
            if( nodename == "advanced_settings" )
            {
                const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
                xmlpp::Element::AttributeList::const_iterator itera;
	       
                for( itera = attributes.begin(); itera != attributes.end(); ++itera)
                {
                    const xmlpp::Attribute* attribute = *itera;
                    Glib::ustring attrname= attribute->get_name();
		
                    std::stringstream ss( attribute->get_value() );
                    if( attrname == "resolution_changeable" )
                        ss >> resolutionChangeable; 
                }
            }
	}
    }

    if( resolutionChangeable )
    {
        std::cout << "[Advanced setting]: Changeable resolution is enabled. Setting static cache resolution to required minimum of 1|1." << std::endl;
        staticCacheXResolution = 1;
        staticCacheYResolution = 1;
    }
}


FwonderConfig* fwonderConf = NULL;
