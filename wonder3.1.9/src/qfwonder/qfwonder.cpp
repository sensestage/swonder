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

#include "qfwonder.h"
#include "cachematrix.h"
#include "osccontrol.h"

#include <QFont>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLCDNumber>
#include <QLabel>
#include <QPalette>

QFWonder::QFWonder( QWidget* parent ) : QWidget( parent )
{   
    // display name of used config file in the window title 
    QString title = "qfwonder - ";
    title.append( qfwonderConf->qfwonderConfigfile.c_str() );
    setWindowTitle( title );

    // the widgets
    currentIRLabel      = new QLabel( "Current IR (x|y): " );
    currentIRValueLabel = new QLabel( "" );
    currentIRLabel->setAutoFillBackground( true ); 
    currentIRLabel->setPalette( QPalette( Qt::green ) );
    currentIRValueLabel->setText( "   0 | 0   " );
    currentIRValueLabel->setMinimumWidth( 60 );
    currentIRLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    currentIRValueLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );

    numStaticIRsLoadedLabel      = new QLabel( "Statically loaded IRs: " );
    numStaticIRsLoadedValueLabel = new QLabel( "" );
    numStaticIRsLoadedLabel->setAutoFillBackground( true ); 
    QPalette staticIRPalette = QPalette( Qt::blue );
    staticIRPalette.setColor( QPalette::WindowText, Qt::white );
    numStaticIRsLoadedLabel->setPalette( staticIRPalette );
    numStaticIRsLoadedValueLabel->setText( "0" );
    numStaticIRsLoadedValueLabel->setMinimumWidth( 60 );
    numStaticIRsLoadedLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );

    numDynamicIRsLoadedLabel      = new QLabel( "Dynamically loaded IRs: " );
    numDynamicIRsLoadedValueLabel = new QLabel( "" );
    numDynamicIRsLoadedLabel->setAutoFillBackground( true ); 
    numDynamicIRsLoadedLabel->setPalette( QPalette( Qt::magenta ) );
    numDynamicIRsLoadedValueLabel->setText( "0" );
    numDynamicIRsLoadedValueLabel->setMinimumWidth( 60 );
    numDynamicIRsLoadedLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );

    cacheMatrix = new CacheMatrix;

    sizeSlider = new QSlider( Qt::Horizontal );
    sizeSlider->setMaximum( 20 );
    sizeSlider->setMinimum( 2 );
    sizeSlider->setValue( 4 );

    // osc input
    connect( oscControl, SIGNAL( currentIR( int, int ) ),             this,                          SLOT( currentIR( int, int ) ) );
    connect( oscControl, SIGNAL( newNumLoadedStaticIRs ( QString ) ), numStaticIRsLoadedValueLabel,  SLOT( setText( QString ) ) );
    connect( oscControl, SIGNAL( newNumLoadedDynamicIRs( QString ) ), numDynamicIRsLoadedValueLabel, SLOT( setText( QString ) ) );
    connect( oscControl, SIGNAL( IRLoaded ( int, int, bool, bool ) ), cacheMatrix,                   SLOT( setIRLoaded( int, int, bool, bool ) ) );
    connect( oscControl, SIGNAL( reset() ),                           this,                          SLOT( reset() ) );

    connect( sizeSlider,  SIGNAL( valueChanged( int ) ), cacheMatrix, SLOT( setDrawSize( int ) ) );

    // set the layout
    mainLayout  = new QVBoxLayout();
    labelLayout = new QHBoxLayout();

    labelLayout->addWidget( currentIRLabel );
    labelLayout->addWidget( currentIRValueLabel );
    labelLayout->addStretch( 1 );
    labelLayout->addWidget( numStaticIRsLoadedLabel );
    labelLayout->addWidget( numStaticIRsLoadedValueLabel );
    labelLayout->addStretch( 1 );
    labelLayout->addWidget( numDynamicIRsLoadedLabel );
    labelLayout->addWidget( numDynamicIRsLoadedValueLabel );
    labelLayout->addStretch( 1 );

    mainLayout->addWidget( cacheMatrix, 1 );
    mainLayout->addLayout( labelLayout );
    mainLayout->addWidget( sizeSlider );

    setLayout( mainLayout );
    
    // set a reasonable default size, because the cachematrix widget has no sizeHints
    resize( 200, 400 );
}

void QFWonder::currentIR( int x, int y )
{
    QString currentIRString = "";
    currentIRString.append( QString::number( x ) );
    currentIRString.append( " | " );
    currentIRString.append( QString::number( y ) );

    currentIRValueLabel->setText( currentIRString );

    cacheMatrix->setCurrentIR( x, y );
}
void QFWonder::reset()
{
    cacheMatrix->resetMatrix();
    currentIRValueLabel->setText( "" );
    numDynamicIRsLoadedValueLabel->setText( "0" );
    numStaticIRsLoadedValueLabel ->setText( "0" );
}


