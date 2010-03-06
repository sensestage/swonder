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

#include "outputarray.h"
#include <sstream>


OutputArray::OutputArray( int noOutputs, int IRPartitionSize, int maxIRLength, int tailPartitionSize, int maxTailLength ) 
{
    for( int i = 1; i <= noOutputs ; ++i )
    {
        std::stringstream outputName;
	outputName << "output" << i;
        // the +1 is to make up for the integer division so that enough buffers will be allocated
	push_back( new OutputAggregate( outputName.str(), IRPartitionSize, ( maxIRLength / IRPartitionSize ) + 1, tailPartitionSize, ( maxTailLength / tailPartitionSize ) + 1  ) );
    }
}

OutputArray::~OutputArray()
{
    while( ! empty() )
    {
        delete back();
        pop_back();
    }
}
