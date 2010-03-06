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

#ifndef OSCCONTROL_H
#define OSCCONTROL_H

#include <QObject>

#include "oscin.h"

#include "qfwonder_config.h"

// arguments of the handler functions
#define handlerArgs const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data

class OSCControl : public QObject, OSCServer
{
    Q_OBJECT

public:
     OSCControl( const char* port, QObject* parent = 0 );

signals:
    void currentIR             ( int x, int y );
    void newNumLoadedDynamicIRs( QString numIRs );
    void newNumLoadedStaticIRs ( QString numIRs );
    void IRLoaded              ( int x, int y, bool loaded, bool dynamicIR );
    void reset                 ();

private:
    static int oscCurrentIRHandler   ( handlerArgs );
    static int oscNumLoadedIRsHandler( handlerArgs );
    static int oscIRLoadedHandler    ( handlerArgs );
    static int oscResetHandler       ( handlerArgs );
    static int oscGenericHandler     ( handlerArgs );

    void addMethods();
};

extern OSCControl* oscControl;

#endif
