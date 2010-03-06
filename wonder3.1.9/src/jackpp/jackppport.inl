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

//---------------------------------------------------------------------------
//  Method: name
//---------------------------------------------------------------------------
 
/*! Returns the (full) name of this port.
 */ 
const char* Port::name() const
{
   return jack_port_name(_jackPort);    
}

int Port::setName(const char* name)
{
   return jack_port_set_name(_jackPort, name);
}

//---------------------------------------------------------------------------
//  Method: shortName
//---------------------------------------------------------------------------

/*! Returns the short name of this port.
 */  
const char* Port::shortName() const
{
   return jack_port_short_name(_jackPort);    
}

//---------------------------------------------------------------------------
//  Method: type
//---------------------------------------------------------------------------

/*! Returns the type of the jack_port_t. 
 */
const char*  Port::type() const
{
    return jack_port_type(_jackPort);
}

//---------------------------------------------------------------------------
//  Method: flags
//---------------------------------------------------------------------------

/*!
    Returns the flags of the jack_port_t.
 */    
int Port::flags() const
{
    return jack_port_flags(_jackPort);
}

/*!
    Returns the time (in frames) between data being available
    or delivered at/to a port, and the time at which it
    arrived at or is delivered to the "other side" of the port.
    
    E.g. for a physical audio output port, this is the time between
    writing to the port and when the audio will be audible.
    For a physical audio input port, this is the time between the sound
    being audible and the corresponding frames being readable from the
    port.  
 */
jack_nframes_t Port::getLatency(void) const
{
    return jack_port_get_latency(_jackPort);
}

/*!
 * This returns a pointer to the memory area associated with the
 * specified port. For an output port, it will be a memory area
 * that can be written to; for an input port, it will be an area
 * containing the data from the port's connection(s), or
 * zero-filled. if there are multiple inbound connections, the data
 * will be mixed appropriately.  
 *
 * FOR OUTPUT PORTS ONLY
 * ---------------------
 * You may cache the value returned, but only between calls to
 * your "blocksize" callback. For this reason alone, you should
 * either never cache the return value or ensure you have
 * a "blocksize" callback and be sure to invalidate the cached
 * address from there.
 */
void* Port::getBuffer( jack_nframes_t nrFrames )
{
    return jack_port_get_buffer(_jackPort, nrFrames );
}

JACKPP_END_NAMESPACE
