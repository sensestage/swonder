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

#ifndef CACHEMATRIX_H
#define CACHEMATRIX_H

#include <QWidget>

class CacheMatrix : public QWidget
{
    Q_OBJECT

    struct IR
    {
        bool loaded;
        bool dynamic;
    };

public:
    CacheMatrix( QWidget* parent = 0 );
    void resetMatrix();

    // indicates type of IR and if it's loaded , contains flags for all possible IRs according 
    // to the fwonder config file used.
    IR* IRMatrix;

    int width;
    int height;
    
    int xMax;
    int xMin;

    int yMax;
    int yMin;


public slots:
    void setCurrentIR( int x, int y );
    void setIRLoaded ( int x, int y, bool on, bool dynamicIR );
    void setDrawSize ( int size );

protected:
    void paintEvent( QPaintEvent *event );

private:
    int  currentIRIndex;
    int  drawSize;
    bool fwonderIsRunning;
};

#endif
