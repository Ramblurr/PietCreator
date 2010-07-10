/*
    Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "ViewMonitor.h"

#include "KColorCells.h"

#include <QDebug>

ViewMonitor::ViewMonitor( QObject* parent ): QObject( parent )
{

    // 3 shades of red
    mColors.insert( 0, QColor( "#FFC0C0" ) );
    mColors.insert( 1, QColor( "#FF0000" ) );
    mColors.insert( 2, QColor( "#C00000" ) );

    // 3 shades of yellow
    mColors.insert( 3, QColor( "#FFFFC0" ) );
    mColors.insert( 4, QColor( "#FFFF00" ) );
    mColors.insert( 5, QColor( "#C0C000" ) );

    // 3 shades of green
    mColors.insert( 6, QColor( "#C0FFC0" ) );
    mColors.insert( 7, QColor( "#00FF00" ) );
    mColors.insert( 8, QColor( "#00C000" ) );

    // 3 shades of cyan
    mColors.insert( 9, QColor( "#C0FFFF" ) );
    mColors.insert( 10, QColor( "#00FFFF" ) );
    mColors.insert( 11, QColor( "#00C0C0" ) );

    // 3 shades of blue
    mColors.insert( 12, QColor( "#C0C0FF" ) );
    mColors.insert( 13, QColor( "#0000FF" ) );
    mColors.insert( 14, QColor( "#0000C0" ) );

    // 3 shades of magenta
    mColors.insert( 15, QColor( "#FFC0FF" ) );
    mColors.insert( 16, QColor( "#FF00FF" ) );
    mColors.insert( 17, QColor( "#C000C0" ) );

    Command first( "", "", mColors.at(0), 0 );
    setCurrentCommand( first );
}

QColor ViewMonitor::currentColor() const
{
    return mStack.top().color;
}

void ViewMonitor::setCurrentColor( int index, const QColor& color )
{
  if( mStack.top().index != index ) {
      mStack.top().color = color;
      mStack.top().index = index;
      emit currentColorChanged( color );
  }
}

void ViewMonitor::setCurrentColor( int index )
{
  if( mStack.top().index != index ) {
      Q_ASSERT( index < mColors.size() );
      mStack.top().color = mColors.at( index );
      mStack.top().index = index;
      emit currentColorChanged( mStack.top().color );
  }
}

int ViewMonitor::pixelSize() const
{
    return mPixelSize;
}

void ViewMonitor::setPixelSize( int size )
{
    mPixelSize = size;
    emit pixelSizeChanged( size );
}

int ViewMonitor::currentColorIndex() const
{
    return mStack.top().index;
}

QColor ViewMonitor::colorForIndex( int index ) const
{
    if( index < mColors.size() )
        return mColors.at( index );
    return Qt::black;
}

QString ViewMonitor::currentCommandLabel() const
{
    return mStack.top().name;
}

Command ViewMonitor::currentCommand() const
{
    return mStack.top();
}


static inline int secondToLastIndex( int size ) {
  return (size > 1) ? size - 2 : size - 1;
}


Command ViewMonitor::takeCommand()
{
    Command c = mStack.pop();
    emit currentCommandChanged( mStack.top(), mStack.at( secondToLastIndex( mStack.size() ) ) );
    if( mStack.last().color != c.color )
        emit currentColorChanged( c.color );
    return c;
}

void ViewMonitor::setCurrentCommand( const Command& command )
{
    mStack.push( command );
    emit currentCommandChanged( mStack.top(), mStack.at( secondToLastIndex( mStack.size() ) ) );
    if( mStack.last().color != command.color )
      emit currentColorChanged( command.color );
}


void ViewMonitor::populateCells( KColorCells* cells )
{
    for( int i = 0; i < mColors.size(); ++i )
        cells->setColor( i, mColors.at( i ) );

    cells->setSelected( 0 );
}



#include "ViewMonitor.moc"


