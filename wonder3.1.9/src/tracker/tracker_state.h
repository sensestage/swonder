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

#ifndef TRACKER_STATE_H
#define TRACKER_STATE_H

#include "isense.h"

class TrackerState
{
public:
    TrackerState(); 
    ~TrackerState();
        
    void initItracker();
    void initPtracker();
    void getITrackerState( float* state );
    void getPTrackerState( float* state );
    void sendTrackerPos( float* pos );
    void showTrackerPos( float* pos );

private:
    int trackerPort;

    ISD_TRACKER_HANDLE    trackerHandle;
    ISD_TRACKER_INFO_TYPE tracker;
};

#endif //TRACKER_STATE_H
