/*
 * (c) Copyright 2006-7 -- Hans-Joachim Mond
 *
 * sWONDER:
 * Wave field synthesis Of New Dimensions of Electronic music in Realtime
 * http://swonder.sourceforge.net
 *
 * created at the Technische Universitaet Berlin, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "XwonderConfig.h"

#include <QRegExp>
#include <QIntValidator>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::string;

//--------------------------------constructors--------------------------------//

XwonderConfig::XwonderConfig( int argc, char** argv, QObject* parent ) : QObject( parent )
{
    // set default values
    cwonderPort       = "58100";
    xwonderPort       = "58400";
    cwonderHost       = "127.0.0.1";
    verbose           = false;
    runWithoutCwonder = false;
    projectOnlyMode   = true; // this is project dependent and will vary at runtime

    name = "xwonder";

    cwonderPingTimeout = 3000;

    // this will be set at runtime by cwonder via OSC
    maxNoSources = 0;  
    roomName = "20m x 20m Demoroom";

    parseArgs( argc, argv );
    cwonderAddr = lo_address_new( cwonderHost, cwonderPort );

    // in demomode maxNoSources will not be set by cwonder
    if( runWithoutCwonder )
        maxNoSources = 15;
}

//----------------------------end of constructors-----------------------------//


void XwonderConfig::parseArgs( int argc, char** argv ) 
{
    // loop through all commandline arguments
    for( int i = 1; i < argc; ++i )
    {
        // for safety reasons we cast the c-strings to QStrings in order to process them
        QString arg( argv[ i ] );

        // if we don't get something like "-x" then exit
        if( ( arg.length() != 2 ) || ( arg[ 0 ] != '-' ) )
            abort( QString( "Unknown argument: " ) + arg );

        // get rid of the '-' and convert to char (for the switch)
        char c = *( arg.right( 1 ).toStdString().c_str() );

        // now parse and validate values
        switch( c )
        {                   
            case 'i': // cwonder IP-address
            {
                ++i;
                if( i == argc )
                    abort( QString( "Missing value for argument: " + arg ) );
                arg = QString( argv[ i ] );
                QRegExp ipRegExp( "^[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}$" );
                if( ! ipRegExp.exactMatch( arg ) )
                    abort( QString( "Invalid IP address: " + arg ) );
                cwonderHostTempString = arg.toStdString();
                cwonderHost           = cwonderHostTempString.c_str();
            }
            break;
            case 'p': // cwonder port
            {
                ++i;
                if( i == argc )
                    abort( QString( "Missing value for argument: " + arg ) );
                arg = QString( argv[ i ] );
                QIntValidator* intValidator = new QIntValidator( 0, 65535, this );
                if( intValidator->validate( arg, i ) != QValidator::Acceptable )
                    abort( QString( "Invalid port: " + arg ) );
                cwonderPortTempString = arg.toStdString();
                cwonderPort           = cwonderPortTempString.c_str();
            }
            break;
            case 'o': // xwonder port
            {
                ++i;
                if( i == argc )
                    abort( QString("Missing value for argument: " + arg ) );
                arg = QString(argv[i]);
                QIntValidator* intValidator = new QIntValidator( 0, 65535 , this );
                if( intValidator->validate( arg, i ) != QValidator::Acceptable )
                    abort( QString( "Invalid port: " + arg ) );
                xwonderPortTempString = arg.toStdString();
                xwonderPort           = xwonderPortTempString.c_str();
            }
            break;
            case 't': // ping timeout
            {
                ++i;
                if( i == argc )
                    abort( QString("Missing value for argument: " + arg ) );
                arg = QString(argv[i]);
                QIntValidator* intValidator = new QIntValidator( 0, 10000, this );
                if( intValidator->validate( arg, i ) != QValidator::Acceptable )
                    abort( QString( "Invalid value for timeout: " + arg ) );
                cwonderPingTimeout  = arg.toInt();
            }
            break;
            case 'n': // name but only 50 characters, should suffice 
                name = arg.left( 50 );
                break;
            case 'd':
                runWithoutCwonder = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'h':
                cout << endl 
                     << "Xwonder arguments:"                                                                            << endl
                     << "-i x (x = host of cwonder, default is 127.0.0.1 )"                                             << endl
                     << "-p x (x = port of cwonder, default is 58100)"                                                  << endl
                     << "-o x (x = port of xwonder, default is 58000)"                                                  << endl
                     << "-t x (x = timeout in ms for connection to cwonder, default is 3000ms, use 0 to turn it off ) " << endl
                     << "-n x (x = name under which to connect to cwonder )"                                            << endl
                     << "-d   (demomode, run without connecting to cwonder)"                                            << endl
                     << "-v   (verbose mode, prints received OSC messages to console)"                                  << endl << endl;
                    exit( EXIT_FAILURE );
                    break;
            default:
                abort( QString( "Unknown argument: " ) + arg );
        }
    }
}


void XwonderConfig::abort( QString message )
{
    cerr << message.toStdString() << endl
         << "Use -h for help"     << endl;
    exit( 1 );
}


XwonderConfig* xwConf = NULL;
