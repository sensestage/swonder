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

#include "osccontrol.h"
#include <iostream>


OSCControl::OSCControl( const char* port, QObject* parent ) : QObject( parent ), OSCServer( port )
{
    addMethods();
    start();
}


void OSCControl::addMethods()
{
    addMethod( "/WONDER/qfwonder/IRLoaded",     "iiii", OSCControl::oscIRLoadedHandler,     this );
    addMethod( "/WONDER/qfwonder/currentIR",    "ii",   OSCControl::oscCurrentIRHandler,    this );
    addMethod( "/WONDER/qfwonder/numLoadedIRs", "ii",   OSCControl::oscNumLoadedIRsHandler, this ); 
    addMethod( "/WONDER/qfwonder/reset",        "",     OSCControl::oscResetHandler,        this ); 
    addMethod( NULL,                            NULL,   OSCControl::oscGenericHandler,      this );  
}


int OSCControl::oscIRLoadedHandler( handlerArgs )
{
    emit oscControl->IRLoaded( argv[ 0 ]->i, argv[ 1 ]->i, argv[ 2 ]->i, argv[ 3 ]->i );
    return 0;
}


int OSCControl::oscCurrentIRHandler( handlerArgs )
{
    emit oscControl->currentIR( argv[ 0 ]->i, argv[ 1 ]->i  );
    return 0;
}

int OSCControl::oscResetHandler( handlerArgs )
{
    emit oscControl->reset();
    return 0;
}

int OSCControl::oscNumLoadedIRsHandler( handlerArgs )
{
    if( argv[ 1 ]->i )
        emit oscControl->newNumLoadedDynamicIRs( QString::number( argv[ 0 ]->i ) );
    else
        emit oscControl->newNumLoadedStaticIRs ( QString::number( argv[ 0 ]->i ) );
    return 0;
}


int OSCControl::oscGenericHandler( handlerArgs )
{
    std::cerr << std::endl << "[qfwonder]: received unknown osc message: " << path << std::endl;
    return 0;
}

OSCControl* oscControl = NULL;
