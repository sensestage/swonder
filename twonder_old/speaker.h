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

#ifndef SPEAKER_H
#define SPEAKER_H

#include "jackppclient.h"
#include "vector2d.h"
#include "vector3d.h"

/**
 * @brief Speaker holds position and orientation of a single speaker. 
 *
 */
class Speaker {
    private:
	static jackpp::Client *jackclient;

	Vector2D pos;
	Vector3D pos3D;
	Vector2D n;
	Vector3D n3D;
	float cos_alpha;

    public:
	static void setJackClient( jackpp::Client *jc ) { jackclient = jc; }
	
	jackpp::Port *port;

/**
 * @brief Constructs Speaker with 3-dim coordinates and orientation plus "bend"-compensation.
 * @param name The speakername
 * @param x x-coordinate
 * @param y y-coordinate
 * @param z z-coordinate
 * @param nx normal x-coordinate 
 * @param ny normal y-coordinate
 * @param nz normal z-coordinate
 * @param cos_alpha cosinus alpha to calculate deviation from speaker plain (speaker plain may be bended)
 */
	Speaker( string name, float x, float y, float z, float nx, float ny, float nz, float cos_alpha );
	
	/*
	Speaker( string name, Vector2D &position, Vector2D &normal ) {

	    if( jackclient ) 
		port = jackclient->registerPort( name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, jackclient->getBufferSize() );
	    pos = position;
	    n = normal;
	}
	*/
	
	const Vector2D &getPos() const  { return pos; }
	const Vector3D &get3DPos() const  { return pos3D; }
	const Vector2D &getN() const { return n; }
	const Vector3D &get3DN() const { return n3D; }
	const float getCosAlpha() const { return cos_alpha; };


};

#endif

