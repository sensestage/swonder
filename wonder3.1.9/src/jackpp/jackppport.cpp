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

#include "jackppport.h"
#include "jackppclient.h"
#include <stdlib.h>

JACKPP_BEGIN_NAMESPACE

/*! \class Port
    \brief Represents a Jack port (jack_port_t).        
 */

//---------------------------------------------------------------------------
//  Method: Port
//---------------------------------------------------------------------------
 
/*! Constructor.
    \param jackPort the jack_port_t to represent.
    
    Ports cannot created directly but instead by a Client instance.
 */
Port::Port(Client *jackClient, jack_port_t *jackPort)
  :  _jackClient(jackClient), 
     _jackPort  (jackPort)
{
}

//---------------------------------------------------------------------------
//  Method: connected
//---------------------------------------------------------------------------

/*! This returns a positive integer indicating the number
    of connections to or from 'port'. 
 
   \pre The calling client must own 'port'.
 */ 
int Port::connected() const
{
        return jack_port_connected(_jackPort);
}

//---------------------------------------------------------------------------
//  Method: connectedTo
//---------------------------------------------------------------------------

/*!
 * This returns TRUE or FALSE if the port argument is
 * DIRECTLY connected to the port with the name given in \e portname
 *
 * \pre The calling client must own 'port'.
 */
bool Port::connectedTo(const char* portname) const
{
        return jack_port_connected_to(_jackPort, portname);
}

//---------------------------------------------------------------------------
//  Method: getConnections
//---------------------------------------------------------------------------

vector<string> Port::getConnections()
{
    const char**   con;
    vector<string> conNames;

    con = jack_port_get_connections(_jackPort);
    
    if( con )
    {
        int i=0;
        while(con[i])
            conNames.push_back( con[i++] );   
            
        free(con);                   
    }
    
    return conNames;
}

//---------------------------------------------------------------------------
//  Method: connectTo
//---------------------------------------------------------------------------

/*! 
   Establishes a connection between this and \e destPort

   When a connection exists, data written to the source port will
   be available to be read at the destination port.

   \pre The types of both ports must be identical to establish a connection.
   \pre The flags of the source port must include PortIsOutput.
   \pre The flags of the destination port must include PortIsInput.

   @return 0 on success, otherwise a non-zero error code
 */
int Port::connectTo(const char* destPort)
{
    return _jackClient->connectPorts(this->name(), destPort);
}

JACKPP_END_NAMESPACE
