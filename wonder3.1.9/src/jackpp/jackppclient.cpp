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

#include "jackppclient.h"
#include "jackppport.h"

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>

//#include <multimap.h>
using namespace std;

JACKPP_BEGIN_NAMESPACE
 
//---------------------------------------------------------------------------
//  Method: Client
//---------------------------------------------------------------------------
   
/*! \brief Constructor.
    Constructs a new Jack client with \e name.
    \param name the name of the new Jack client
 */   
Client::Client(const char* name)
    :   _name(name), _jackClient(NULL)
{

}


//---------------------------------------------------------------------------
//  Method: ~Client
//---------------------------------------------------------------------------

/*! \brief Destructor.

    Unregisters all own ports and clears the own PortMap.
    Then it deletes all alien Port instances and clears the alien PortMap.
    Finally it deactives the client and disconnects from the Jack server.
 */
Client::~Client()
{
    vector<Port*> ports = getOwnPorts();
    vector<Port*>::iterator it = ports.begin();
    
    // unregister own port
    while( it != ports.end() ) {
        unregisterPort(*it);
        it++;
    }    
    _ownPorts.clear();    

    // delete alien Port instances
    PortMap::iterator apit;
    apit = _alienPorts.begin();
    while( apit != _alienPorts.end() ) {
        cerr << "jackpp: deleting alien port map entry [" 
             << apit->second->name() << "]\n";
        delete (apit->second);
        apit++;
    }          
    _alienPorts.clear();   

    deactivate();
    disconnect();
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

/*! \fn Client::connect(const char*)
    Tries to connect and become a client of the Jack server.
        \param n the name of the client passed to jack_client_new()
    \return \b true, if connection succeeded,\n
            \b false, if connection failed
 */                


bool Client::connect( const char* n )
{
    string name;

    if( n )
        name = n;
    else
        name = _name;
    
    _jackClient = jack_client_new(name.c_str());
   
    if( !_jackClient ) {
      cerr << "jackpp: Error connecting client [" << name << "] to server.\n";
      return false;
    }
            
    //cout << "jackpp: connected client [" << _name << "] to JACK server \n";
    return true;    
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

/*! \fn Client::disconnect
    Disconnects this client from the Jack server.
    \return the error value or 0 if no error
 */

int Client::disconnect()
{
    int error = 0;
    
    if( _jackClient )
        error = jack_client_close( _jackClient );
      
          
    return error;  
    //return error ? false : true;        
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

/*! \fn Client::activate()
    Tells the Jack server that this client is ready to start 
    processing audio.         
 */


int  Client::activate()
{
    int ret = jack_activate( _jackClient );
    //cout << "jackpp: activated client [" << _name << "]" << endl;
    return ret;
    
}


//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------


/*! \fn Client::deactivate()
    Tells the Jack server that the program should be removed from the 
    processing graph. As a side effect, this will disconnect any and 
    all ports belonging to the client, since inactive clients are not 
    allowed to be connected to any other ports. 
 */

int  Client::deactivate()
{
    if( _jackClient ) {
        int ret = jack_deactivate( _jackClient );
        return ret;
    }
    return -1;                
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------


/*! \fn Client::registerPort( const string& name,  
                        const string& type = "JACK_DEFAULT_AUDIO_TYPE",
                        unsigned long flags = JackPortIsOutput | 
                                              JackPortIsTerminal, 
                        unsigned long buffer_size = 0 )
    \param name the name of the new port
    \param type the type of the port as a string
    \param flags bitmask of JackPortFlags values

    Tries to register a new port called \e name. The port will be of
    type JACK_DEFAULT_AUDIO_TYPE if not differently set via \e type.
    The default \e flags register an output port serving as terminal.
    
    This creates a new port for the client. A port is an object used for 
    moving data in or out of the client. the data may be of any type. 
    Ports may be connected to each other in various ways. A port has a short 
    name, which may be any non-NULL and non-zero length string, and is passed 
    as the first argument. A port's full name is the name of the client 
    concatenated with a colon (:) and then its short name.

    A port has a \e type, which may be any non-NULL and non-zero length string, 
    and is passed as the second argument. For types that are not built into the
     jack API (currently just JACK_DEFAULT_AUDIO_TYPE) the client MUST supply 
     a non-zero size for the buffer as for \e buffer_size. For builtin types, 
     \e buffer_size is ignored.

    The \e flags argument is formed from a bitmask of JackPortFlags values.
 */
Port* Client::registerPort( const string& name,  
                            const char*   type,
                            unsigned long flags, 
                            unsigned long buffer_size )
{    
    // try to register the port
    jack_port_t  *jackPort = jack_port_register(_jackClient,
                                                name.c_str(), 
                                                type, 
                                                flags, 
                                                buffer_size);
    
    // if an error occured return the null port
    if( !jackPort ) {
        cerr << "jackpp: error registering port [" << name << "]\n";
        return 0;
    }
    
    // insert a new pointer into the ownPort map
    string fullname(_name+":");
    fullname+=name;
    pair<PortMap::iterator, bool> result;
    result = 
      _ownPorts.insert(pair<const string,Port*>(fullname,new Port(this,jackPort)));
    if( !result.second ) {
        cerr << "jackpp: Error registering. Port [" << fullname 
             << "] already exists.\n";
        return (*result.first).second;             
    }
        
    return (*result.first).second;          
}                   

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------


/*! \fn Client::unregisterPort( Port* port )
    \param port the port to remove
    \return \b true, if the port was succesfully removed,
            \b false, if an error occured
            
    This removes the port from the client, disconnecting any existing 
    connections at the same time. 
 */   
int Client::unregisterPort( Port* port )
{
    int error;
    PortMap::iterator it;
    
    if( (it = _ownPorts.find(port->name())) == _ownPorts.end() ) {
        //cerr << "jackpp: unregisterPort(): port [" << port->name()
        //     << "] not registered.\n";
        return -1;
    }        
            
    error = jack_port_unregister(_jackClient, port->_jackPort);
    if( error ) 
        return error;
       
    _ownPorts.erase(it);
        
    return 0;       
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------


/*! \fn Client::getOwnPorts()
    \return a vector containing the ports owned by this client
    
    Returns a vector containing the ports owned by this client.
 */
vector<Port*> Client::getOwnPorts()
{
    PortMap::iterator it = _ownPorts.begin();
    vector<Port*> ports;
    
    while( it != _ownPorts.end() )
    {
        ports.push_back(it->second);
        it++;
    }
    return ports;
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------


/*! \fn Client::getPortByName(const string& name)
    \param name the name of the port to get
    \return the port with \e name, if it exists,\n
            NULL if no port with that name exists
            
    Searches the port with \e name and returns it.            
 */
Port* Client::getPortByName( const string& name )
{
    PortMap::iterator it;
    
    // if port is already in ownMap, return it
    if( (it = _ownPorts.find(name)) != _ownPorts.end() ) 
        return it->second;
        
    // if port is already in alienMap, return it
    if( (it = _alienPorts.find(name)) != _alienPorts.end() ) 
        return it->second;
    
    // port must be alien
    jack_port_t *port = jack_port_by_name( _jackClient, name.c_str() );
    
    if( port ) {
        Port *p = new Port(0,port);
        _alienPorts[name] = p;
        cerr << "jackpp: added alien port [" << name << "]" << endl;
        return p;
    }
    
    cerr << "jackpp: no port [" << name << "] found.\n" << flush;
    
    return 0;
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------


/*! \fn Client::getPortNames(const string& portNamePattern=string(),
                                const string& typeNamePattern=string(),
                                unsigned long flagPattern=0 )
                                
    Returns a vector containing the names of the ports available at the
    Jack server.
 */                                
vector<string> Client::getPortNames(const string& portNamePattern,
                                const string& typeNamePattern,
                                unsigned long flagPattern )
{
    vector<string> portNames;
    const char**   ports = jack_get_ports(_jackClient, 
                                          portNamePattern.c_str(),
                                          typeNamePattern.c_str(), 
                                          flagPattern);
    
    if( ports ) {
        int i=0;
        while( ports[i] ) 
            portNames.push_back( ports[i++] );                
    
        free( ports );        
    }
    else
        cerr << "jackpp: Error getting port names.\n";

    return portNames;    
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

vector<string> Client::getClientNames(void) 
{
    vector<string> portNames = getPortNames();
    vector<string> clientNames;
    string         clientName;
    
    vector<string>::const_iterator it = portNames.begin();
    
    while( it != portNames.end() )
    {
        clientName = (*it).substr(0, (*it).find_first_of(':'));
        if( find(clientNames.begin(),clientNames.end(), clientName) 
            == clientNames.end())
            clientNames.push_back(clientName);
        it++;    
    }
    
    return clientNames;
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

multimap<string,string> 
Client::getPortsByClients(const vector<string>& portNames)
{
//    Port *port=0;
    multimap<string,string> mm;
    vector<string>::const_iterator it = portNames.begin();
    string prefix, name;
    
    while(it != portNames.end() )
    {
        prefix = (*it).substr(0,(*it).find_first_of(':'));
        name   = (*it).substr((*it).find_last_of(':')+1,(*it).length());
        mm.insert( pair<string,string>(prefix,name) );
        it++;
    }
    
    return mm;
}

JACKPP_END_NAMESPACE
