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

#include "speaker.h"
#include "twonder_config.h"
#include <iostream>

using std::cout;
using std::endl;

jackpp::Client* Speaker::jackclient = NULL;

Speaker::Speaker( string name, float x, float y, float z, float normalx, float normaly, float normalz, float cosAlpha )
{
    if( twonderConf->verbose )
    {
        cout << "spk: "       << name 
             << " pos [ "     << x       << ", " << y       << ", " << z
             << " ] normal [ " << normalx << ", " << normaly << ", " << normalz << " ]" 
             << endl;
    }
    
    if( jackclient ) 
        port = jackclient->registerPort( name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, jackclient->getBufferSize() );

    pos[ 0 ] = x;
    pos[ 1 ] = y;

    pos3D[ 0 ] = x;
    pos3D[ 1 ] = y;
    pos3D[ 2 ] = z;

    normal[ 0 ] = normalx;
    normal[ 1 ] = normaly;
    
    normal3D[ 0 ] = normalx;
    normal3D[ 1 ] = normaly;
    normal3D[ 2 ] = normalz;

    this->cosAlpha = cosAlpha;
}
