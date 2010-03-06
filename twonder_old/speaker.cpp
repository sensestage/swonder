/*
 * (c) Copyright 2006-7 -- Torben Hohn
 *
 * sWONDER:
 * Wave field synthesis Of New Dimensions of Electronic music in Realtime
 * http://swonder.sourceforge.net
 *
 * created at the Technische Universit�t Berlin, Germany
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

#include "speaker.h"
#include "twonder_globalconfig.h"
#include <iostream>

jackpp::Client *Speaker::jackclient = NULL;

Speaker::Speaker( string name, float x, float y, float z, float nx, float ny, float nz, float ca ) {

    if( conf->verbose ) {
	std::cout << "spk: " << name 
	    << " pos [ " << x << " " << y 
	    << "] norm [ " << nx << " " << ny << " ]" 
	    << std::endl;
    }
    
    if( jackclient ) 
	port = jackclient->registerPort( name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, jackclient->getBufferSize() );

    pos[0] = x;
    pos[1] = y;

    pos3D[0] = x;
    pos3D[1] = y;
    pos3D[2] = z;

    n[0] = nx;
    n[1] = ny;
    
    n3D[0] = nx;
    n3D[1] = ny;
    n3D[2] = nz;

    cos_alpha = ca;
}
