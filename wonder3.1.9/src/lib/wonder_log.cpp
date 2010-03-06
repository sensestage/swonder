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

#include "wonder_log.h"

WonderLog::WonderLog()
{
    openlog( "", LOG_CONS, LOG_LOCAL7 );
}


WonderLog::WonderLog( const char *name )
{
    openlog( name, LOG_CONS, LOG_LOCAL7 );
}


WonderLog::~WonderLog()
{
    closelog();
}


void WonderLog::setMode( int mode )
{
    if( mode == 0 ) // error
        setlogmask( LOG_MASK( LOG_ERR ) );

    else if( mode == 1 ) // debug
        setlogmask( LOG_MASK( LOG_ERR ) | LOG_MASK( LOG_DEBUG )  );

    else if( mode == 2 ) // verbose
        setlogmask( LOG_MASK( LOG_ERR ) | LOG_MASK( LOG_DEBUG ) | LOG_MASK( LOG_INFO ) );
}


void WonderLog::print( int priority, std::string msg )
{
    syslog( priority, msg.c_str() );
}
