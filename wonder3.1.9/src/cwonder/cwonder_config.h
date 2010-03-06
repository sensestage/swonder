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

#ifndef CWONDER_CONFIG_H
#define CWONDER_CONFIG_H

#include "wonder_log.h"
#include "vector3d.h"

#include <vector>
#include <string>


namespace xmlpp
{
    class Node;
}


class CwonderConfig
{

public:
    CwonderConfig( int argc, char** argv );

    int readConfig();

    bool verbose;
    bool oscverbose;
    bool daemon;
    bool basicMode;

    int pingRate;

    std::string cwonderConfigFile;

    const char* user;
    const char* listeningPort;

    std::string projectPath;
    int maxNoSources;

    std::string roomName;
    std::vector< Vector3D > renderPolygonPoints;

private:
    void parseArgs( int argc, char** argv );
    void getSettings( xmlpp::Node* node );
    void getRenderPolygon( xmlpp::Node* node );
};

extern CwonderConfig* cwonderConf;
extern WonderLog*     wonderlog;

#endif
