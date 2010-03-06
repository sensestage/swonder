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

JACKPP_BEGIN_NAMESPACE

/*! 
    Returns 1 if the jack_port_t belongs to the jack_client_t. 
 */
bool  Client::hasOwnPort(Port* port )
{
    PortMap::iterator it;
    if( (it = _ownPorts.find(port->name())) != _ownPorts.end() )
        return true;
    else    
        return false;    
    
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

/*! 
   Establishes a connection between two ports.

   When a connection exists, data written to the source port will
   be available to be read at the destination port.

   \param scrPort the source port
   \param destPort the destination port
   \pre The types of both ports must be identical to establish a connection.
   \pre The flags of the source port must include PortIsOutput.
   \pre The flags of the destination port must include PortIsInput.

   \return 0 on success, otherwise a non-zero error code        
 */                                        
int Client::connectPorts(Port* srcPort, Port* destPort)
{
    return connectPorts(srcPort->name(), destPort->name());
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

/*! 
   Establishes a connection between two ports.

   When a connection exists, data written to the source port will
   be available to be read at the destination port.

   \pre The types of both ports must be identical to establish a connection.
   \pre The flags of the source port must include PortIsOutput.
   \pre The flags of the destination port must include PortIsInput.

   \return 0 on success, otherwise a non-zero error code        
 */         
int Client::connectPorts(const char* srcPort, const char* destPort)
{
    return jack_connect(_jackClient, srcPort, destPort);
}

//---------------------------------------------------------------------------
//  Method:
//---------------------------------------------------------------------------

/*!
 * Removes a connection between two ports.
 *
 * \pre The types of both ports must be identical to establish a connection.
 * \pre The flags of the source port must include PortIsOutput.
 * \pre The flags of the destination port must include PortIsInput.
 *
 * \return 0 on success, otherwise a non-zero error code
 */ 
int Client::disconnectPorts(const char* srcPort, const char* destPort)
{
    return jack_disconnect(_jackClient, srcPort, destPort);
}

//---------------------------------------------------------------------------
//  Method: disconnectPorts
//---------------------------------------------------------------------------

/*!
 * Removes a connection between two ports.
 *
 * \pre The types of both ports must be identical to establish a connection.
 * \pre The flags of the source port must include PortIsOutput.
 * \pre The flags of the destination port must include PortIsInput.
 *
 * \return 0 on success, otherwise a non-zero error code
 */ 
int Client::disconnectPorts(Port* srcPort, Port* destPort)
{
    return connectPorts(srcPort->name(), destPort->name());
}

//---------------------------------------------------------------------------
//  Method: isRealtime
//---------------------------------------------------------------------------

/*! Check if the JACK subsystem is running with -R (--realtime).
    
    \return 1 if JACK is running realtime, -1 if JACK client isn't initialized,
            0 otherwise
*/
int Client::isRealtime() const
{
    if( _jackClient )
        return jack_is_realtime(_jackClient);
    else
        return -1;
}


//---------------------------------------------------------------------------
//  Method: setProcessCallback
//---------------------------------------------------------------------------

int Client::setProcessCallback(JackProcessCallback cb, void *arg)
{
    return jack_set_process_callback(_jackClient, cb, arg);
}

//---------------------------------------------------------------------------
//  Method: setBufferSizeCallback
//---------------------------------------------------------------------------

int Client::setBufferSizeCallback(JackBufferSizeCallback cb, void *arg)
{
    return jack_set_buffer_size_callback(_jackClient, cb, arg );
}

//---------------------------------------------------------------------------
//  Method: getBufferSize
//---------------------------------------------------------------------------

/*!
  This returns the current maximum size that will ever be passed to
  the @a process_callback.  It should only be used *before* the
  client has been activated.  This size may change, clients that
  depend on it must register a @a bufsize_callback so they will be
  notified if it does.
   
 */
jack_nframes_t Client::getBufferSize() const
{
    return jack_get_buffer_size(_jackClient);
}

jack_nframes_t Client::getFrameTime() const
{
    jack_position_t pos;
    jack_transport_query( _jackClient, &pos );
    return pos.frame;
}


//---------------------------------------------------------------------------
//  Method: getSampleRate
//---------------------------------------------------------------------------
/*!
 * This returns the sample rate of the jack system, as set by the user when
 * jackd was started.
 */
jack_nframes_t Client::getSampleRate() const
{
	return jack_get_sample_rate(_jackClient);
}


JACKPP_END_NAMESPACE
