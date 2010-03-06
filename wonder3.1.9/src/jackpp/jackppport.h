/***************************************************************************
 *   Copyright (C) 2004 by Peter Eschler                                   *
 *   peschler@t-online                                                     *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2.1 of the    *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Lesser General Public License for more details.                     *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with this program; if not, write to the Free Software     *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,                  *
 * MA 02111-1307, USA.                                                     *
 ***************************************************************************/

#ifndef _JACKPP_PORT_H_
#define _JACKPP_PORT_H_

#include "jackppdefines.h"

#include <vector>
#include <string>
using namespace std;

#include <jack/jack.h>

JACKPP_BEGIN_NAMESPACE

class Client;

class Port {

    friend class Client;
    
public:
    inline const char* name     (void) const;
    inline int         setName  (const char* name);
    inline const char* shortName(void) const;    
    inline const char* type     (void) const;
    inline int         flags    (void) const; 
    inline jack_nframes_t getLatency(void) const;

        int  connected() const;
//      bool connectedTo(const Port* port) const;
        bool connectedTo(const char* portname) const;
    int  connectTo(const char* destPort);
    
    vector<string> getConnections();
    
    inline void* getBuffer( jack_nframes_t nrFrames );
    
   
protected:
    Port(Client* jackClient, jack_port_t *jackPort);    
    
private:
    Client            *_jackClient;
    jack_port_t       *_jackPort;

};

JACKPP_END_NAMESPACE

#include "jackppport.inl"

#endif //_JACKPP_CLIENT_H_
