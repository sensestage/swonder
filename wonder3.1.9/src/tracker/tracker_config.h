 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  *                                                                                   *
  *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
  *  http://swonder.sourceforge.net                                                   *
  *                                                                                   *
  *                                                                                   *
  *  (c) Copyright 2006-8                                                             *
  *  Berlin Institute of Technology, Germany                                          *
  *  Audio Communication Group                                                        *
  *  www.ak.tu-berlin.de                                                              *
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

#ifndef TRACKERCONFIG_H
#define TRACKERCONFIG_H

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>
#include <lo/lo.h>

#include <list>
#include <string>


enum TrackerType
{
    ITRACKER,
    PTRACKER
};

// struture containing data for a client to which osc messages should be sent
// lo_address handling is encapsulated by designated functions  
struct OSCClient
{
    std::string oscHost;
    std::string oscPort;
    lo_address address;

    bool sendPan;
    bool sendTilt;
    bool sendRot;

    void initAddress()
    {
        address = lo_address_new( oscHost.c_str(), oscPort.c_str() );
    }

    void releaseAddress()
    { 
        lo_address_free( address );
    }
};


class TrackerConfig
{

public:
    TrackerConfig( int argc, char** argv );
    ~TrackerConfig();

    /**
     * \brief           Read the configuration data from the configuration file 
     * \return          0=succesful, 1=config file does not exist, 2=dtd file does not exist, 3=dtd error, 4=xml error
     */
    int readConfig();

    void print();

    const char* listeningPort;

    std::string trackerConfigfile;

    TrackerType trackerType;
    std::string trackerName;

    int omit;
    
    // for slowing down tracker_app (only needed for itracker), --> uwait(wait)
    int wait;
    
    std::list< OSCClient > clients;

    // flag for console output of data
    bool verbose;

    // flags for latency testing
    bool latencyTestMode;
    bool writeLatencyFile;

    // how many time measurements should be made when latency testing
    int testPoints;

    // where the results of the latency testing should be stored
    std::string latencyTestFileName;

private:

    void parse_args( int argc, char** argv );

    /// get the tracker settings from the dom representation
    void getTrackerSettings( xmlpp::Node* node );
    
    /// get the send settings from the dom representation
    void getOSCClientSettings( xmlpp::Node* node );

    /// get the tail settings from the dom representation
    void parseArgs( int argc, char** argv );
};

extern TrackerConfig* trackerConf;

#endif // TRACKERCONFIG_H
