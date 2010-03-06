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

#ifndef IRMATRIXMANAGER_H
#define IRMATRIXMANAGER_H

#include "irmatrix.h"

#include <string>
#include <pthread.h>

class ImpulseResponse;

class IRMatrixManager : public IRMatrix 
{
public:
    IRMatrixManager( int IRPartitionSize, int noIRParts, std::string IRPath, std::string tailPath );
    
    ImpulseResponse* getBestIR();

    bool manage();

    void setPanAndTilt( float pan, float tilt );
    void setPan       ( float pan  );
    void setTilt      ( float tilt );

    ImpulseResponse* getTail( int tailPartitionSize, int noTailPartitions, int maxIRLength );

private:
    void loadStaticIRs();

    std::string getFileName( int x, int y );
    std::string getTailName();

    int IRPartitionSize;
    int maxNoPartitions;

    int dynamicCacheMaxNoIRs;

    std::string IRPath;
    std::string tailPath;

    // need mutexlock
    float targetPosX;
    float targetPosY;
    int   directionX; // +-1 or 0 
    int   directionY; // +-1 or 0 


};

#endif
