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
#include <QDebug>
ViewMonitor::ViewMonitor( QObject* parent ): QObject( parent )
{

}

QColor ViewMonitor::currentColor() const
{
    return mCurrentColor;
}

void ViewMonitor::setCurrentColor( const QColor& color )
{
    qDebug() << "OMG HI";
    qDebug() << color.rgb();
    mCurrentColor = color;
    emit currentColorChanged( color );
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



#include "ViewMonitor.moc"


