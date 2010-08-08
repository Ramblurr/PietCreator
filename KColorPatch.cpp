/*
    Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
    This file was part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 2007 Roberto Raggi (roberto@kdevelop.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#include "KColorPatch.h"

#include "KColorMimeData.h"

#include <QtGui>

class KColorPatch::KColorPatchPrivate
{
public:
    KColorPatchPrivate( KColorPatch *q ): q( q ) {}

    KColorPatch *q;
    QColor color;
};

KColorPatch::KColorPatch( QWidget *parent ) : QFrame( parent ), d( new KColorPatchPrivate( this ) )
{
    setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    setAcceptDrops( true );
    setMinimumSize( 12, 12 );
}

KColorPatch::~KColorPatch()
{
    delete d;
}

QColor KColorPatch::color() const
{
    return d->color;
}

void KColorPatch::setColor( const QColor &col )
{
    QColor old = d->color;
    d->color = col.toRgb();

    update();
    emit colorChanged( d->color, old );
}

void KColorPatch::paintEvent( QPaintEvent* pe )
{
    QFrame::paintEvent( pe );
    QPainter painter( this );

    painter.fillRect( contentsRect(), d->color );
}

void KColorPatch::mouseMoveEvent( QMouseEvent *e )
{
    // Drag color object
    if ( !( e->buttons() & Qt::LeftButton ) )
        return;
    KColorMimeData::createDrag( d->color, this )->start();
}

void KColorPatch::dragEnterEvent( QDragEnterEvent *event )
{
    event->setAccepted( KColorMimeData::canDecode( event->mimeData() ) );
}

void KColorPatch::dropEvent( QDropEvent *event )
{
    QColor c = KColorMimeData::fromMimeData( event->mimeData() );
    if ( c.isValid() ) {
        QColor old = d->color;
        setColor( c );
        emit colorChanged( c, old );
    }
}

#include "KColorPatch.moc"
