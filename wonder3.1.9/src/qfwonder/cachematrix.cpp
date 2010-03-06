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

#include <QPainter>
#include <QColor>
#include "cachematrix.h"
#include "qfwonder_config.h"

#include <iostream>

using std::cerr;
using std::endl;

CacheMatrix::CacheMatrix( QWidget* parent ) : QWidget( parent )
{
    fwonderIsRunning = false;

    xMin = qfwonderConf->sourceAzimuthStart;
    xMax = qfwonderConf->sourceAzimuthStop;

    yMin = qfwonderConf->sourceElevationStart;
    yMax = qfwonderConf->sourceElevationStop;

    width  = xMax - xMin + 1;
    height = yMax - yMin + 1;

    setPalette( QPalette( QColor( 0, 0, 0 ) ) );
    setAutoFillBackground( true );

    IRMatrix = new IR[ width * height ]; 

    drawSize = 4;

    currentIRIndex = 0;

    resetMatrix();

    setMinimumSize( 50, 200 );
}

void CacheMatrix::setCurrentIR( int x, int y )
{
    fwonderIsRunning = true;

    currentIRIndex = x - xMin + ( y - yMin ) * width ;

    // make sure index is valid, otherwise reset to the center
    if( currentIRIndex < 0 || currentIRIndex >= ( width * height ) )
        currentIRIndex = - xMin - yMin * width;

    update();
}


void CacheMatrix::setIRLoaded( int x, int y, bool loaded, bool dynamicIR )
{
    // check if a static IR was unloaded, this should never happen in fwonder...
    if( ! loaded && ! dynamicIR )
        cerr << "ERROR! Fwonder removed a static IR, this is an error, please report it!" << endl;

    int loadedIRIndex = x - xMin + ( y - yMin ) * width ;

    // make sure index is valid, otherwise do nothing
    if( loadedIRIndex < 0 || loadedIRIndex >= ( width * height ) )
        return;

    IRMatrix[ loadedIRIndex ].loaded  = loaded;
    IRMatrix[ loadedIRIndex ].dynamic = dynamicIR;
    update();
}


void CacheMatrix::resetMatrix()
{
    // set whole matrix to not loaded
    for( int i = 0; i < width * height; ++i )
    {
        IRMatrix[ i ].loaded  = false;
        IRMatrix[ i ].dynamic = false;
    }

    update();
}

void CacheMatrix::setDrawSize( int size )
{
    int minSize = 2;
    int maxSize = 20;

    if( size < minSize || size > maxSize )
        return;

    drawSize = size;
    update();
}


void CacheMatrix::paintEvent( QPaintEvent* /* event */ )
{
    QPainter painter( this ); 

    qreal cornerRadius = 0;
    int   drawStep     = drawSize + 2;

    // draw the IR matrix linewise, starting on the upper left corner
    for( int i = xMin; i <= xMax; ++i )
    {
        int xDrawPos = i * drawStep + drawStep * ( 1 - xMin );

        for( int j = yMin; j <= yMax; ++j )
        {
            int yDrawPos = ( yMax + 1 ) * drawStep - ( j * drawStep ); 

            int currIndex = i - xMin + ( j - yMin ) * width;

            // set color
            // gray                = not loaded
            // magenta             = dynamically loaded
            // blue                = statically loaded
            // green               = IR currently in use (using dynamically loaded IR )
            // green + blue border = IR currently in use (using dynamically loaded IR )
            if( currIndex == currentIRIndex  &&  fwonderIsRunning )
            {
                if( ! IRMatrix[ currIndex ].dynamic )
                    painter.setPen( Qt::blue );
                else
                    painter.setPen( Qt::green );

                painter.setBrush( Qt::green );
            }
            else if( IRMatrix[ currIndex ].loaded )
            {
                if( IRMatrix[ currIndex ].dynamic )
                {
                    painter.setPen  ( Qt::magenta );
                    painter.setBrush( Qt::magenta );
                }
                else
                {
                    painter.setPen  ( Qt::blue );
                    painter.setBrush( Qt::blue );
                }

            }
            else
            {
                painter.setPen  ( Qt::lightGray );
                painter.setBrush( Qt::lightGray );
            }

            painter.drawRoundedRect( xDrawPos, yDrawPos, drawSize, drawSize, cornerRadius, cornerRadius );
        }
    }
}
