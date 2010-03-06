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

#ifndef FWONDERCONFIG_H
#define FWONDERCONFIG_H

#include <string>
#include <lo/lo.h>

namespace xmlpp
{
    class Node;
}


enum WindowType
{
    LINEAR,
    COS2,
    NOWIN
};

/**
 * \brief Read the settings from the fwonder config file
 *
 * This class provides the facilities to read the fwonder config file. 
 */

class FwonderConfig
{
public:
    FwonderConfig( int argc, char** argv );
    ~FwonderConfig();

    /**
     * \brief           Get the settings from the fwonder config file 
     * \return          0=succesful, 1=config file does not exist, 2=dtd file does not exist, 3=dtd error, 4=xml error
     */
    int readConfig();

    // fwonder Settings
    std::string fwonderConfigfile;

    int  sampleRate;

    bool IRVerbose;
    bool oscResolutionVerbose;
    bool oscMoveVerbose;

    //Jack Settings
    std::string jackName;
    int noSources;
    int noOutputs;

    //Impulseresponse Settings
    bool doCrossfades;
    bool useTail;
    int  maxIRLength;

    //Tail Settings
    bool        doTailCrossfades;
    int         tailPartitionSize;
    int         maxTailLength;
    int         tailOffset;
    WindowType  tailWindow; 
    std::string tailName;

    //Dynamic Cache Settings
    int  dynamicCacheXResolution;
    int  dynamicCacheYResolution;
    int  dynamicCacheXRadius;
    int  dynamicCacheYRadius;

    //Static Cache Settings
    int staticCacheXResolution;
    int staticCacheYResolution;

    // advanced settings
    bool resolutionChangeable;
    
    //BRIR Settings
    std::string BRIRPath;
    int sourceElevationStart;
    int sourceElevationStop;
    int sourceAzimuthStart;
    int sourceAzimuthStop;
  
    // osc port for incoming messages
    std::string listeningPort;  

    // address of qfwonder
    lo_address  qfwonderAddr;
    const char* qfwonderHost;
    const char* qfwonderPort;

private:
    void parseArgs( int argc, char** argv );

    void getJackSettings    ( xmlpp::Node* node );
    void getBRIRSettings    ( xmlpp::Node* node );
    void getIRSettings      ( xmlpp::Node* node );
    void getTailSettings    ( xmlpp::Node* node );
    void getAdvancedSettings( xmlpp::Node* node );
};

extern FwonderConfig* fwonderConf;

#endif
