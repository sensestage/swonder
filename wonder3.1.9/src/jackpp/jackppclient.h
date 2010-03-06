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

#ifndef _JACKPP_CLIENT_H_
#define _JACKPP_CLIENT_H_

#include <vector>
#include <string>
#include <map>
using namespace std;

#include "jackppdefines.h"
#include "jackppport.h"

#include <jack/jack.h>

JACKPP_BEGIN_NAMESPACE

class Client {

public:
    /*! A map with string keys associated to Ports. */
    typedef map<string,Port*> PortMap;

    Client(const char* name="MyClient");  
    virtual ~Client();

    bool  connect( const char* name=0 );    
    int   disconnect();    
    int   activate();    
    int   deactivate();
    
    Port* registerPort( const string& name,  
                        const char*   type = JACK_DEFAULT_AUDIO_TYPE,
                        unsigned long flags = JackPortIsOutput, 
                        unsigned long buffer_size = 0 );
                        
    int            unregisterPort( Port* port );
    inline bool    hasOwnPort    (Port* port );    
    vector<Port*>  getOwnPorts   (void);        
    Port*          getPortByName (const string& name);    
    vector<string> getPortNames  (const string& portNamePattern=string(),
                                  const string& typeNamePattern=string(),
                                  unsigned long flagPattern=0 );
    vector<string> getClientNames(void);   
                                  
    inline int     connectPorts  (Port* srcPort, Port* destPort);
    inline int     disconnectPorts(Port* srcPort, Port* destPort);
    
    inline int     connectPorts  (const char* srcPort, const char* destPort);
    inline int     disconnectPorts(const char* srcPort, const char* destPort);
    
    inline int     isRealtime() const;                

    inline int     setProcessCallback(JackProcessCallback cb, void *arg = 0);
    inline int    setBufferSizeCallback(JackBufferSizeCallback cb, void *arg=0); 

    inline jack_nframes_t getFrameTime() const;
    
    inline jack_nframes_t getBufferSize() const;
        inline jack_nframes_t getSampleRate() const;
    
    static multimap<string,string> getPortsByClients(const vector<string>& portNames);
    
private:
    string               _name;
    jack_client_t       *_jackClient;   

    PortMap              _ownPorts;
    PortMap              _alienPorts;
};

/*! \class Client
    \brief Represents a Jack client.
      
 */

JACKPP_END_NAMESPACE

#include "jackppclient.inl"

#endif //_JACKPP_CLIENT_H_
