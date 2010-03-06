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

#include <QApplication>

#include <iostream>

#include "qfwonder_config.h"
#include "osccontrol.h"
#include "qfwonder.h"

using std::cerr;
using std::endl;

int main( int argc, char* argv[] )
{
    qfwonderConf = new QFwonderConfig( argc, argv );

    int retVal = qfwonderConf->readConfig(); 

    if( retVal != 0 )
    {
        cerr << "Error parsing qfwonder configfile!" << endl;
        switch( retVal )
        {
            case 1:
                cerr << "configfile " << qfwonderConf->qfwonderConfigfile << " does not exist." << endl;
            break;
            case 2:
                cerr << "dtd file" << " could not be found. " << endl;
            break;
            case 3:
                cerr << "configfile " << qfwonderConf->qfwonderConfigfile << " is not well formed." << endl;
            break;
            case 4:
                cerr << "libxml caused an exception while parsing " << qfwonderConf->qfwonderConfigfile << endl;
            break;
            default:
            cerr << " an unkown error occurred, sorry." << endl;
        }
        exit( EXIT_FAILURE );
    }

    QApplication theApp( argc, argv );

    /// setup OSC server 
    try
    {
        oscControl = new OSCControl( qfwonderConf->listeningPort.c_str() );    
    }
    catch( OSCServer::EServ )
    {
        cerr << "[E-OSCServer] Could not create server, maybe the server( using the same port ) is already running?" << endl;                   
        exit( EXIT_FAILURE );
    }

    QFWonder* mainWindow = new QFWonder();

    mainWindow->show();
    return theApp.exec();
}
