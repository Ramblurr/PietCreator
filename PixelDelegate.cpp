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

#include "PixelDelegate.h"

#include "ViewMonitor.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

PixelDelegate::PixelDelegate( ViewMonitor* monitor, QObject *parent ) : QAbstractItemDelegate( parent ), mMonitor( monitor )
{
}

void PixelDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{

    if ( option.state & QStyle::State_Selected )
        painter->fillRect( option.rect, option.palette.highlight() );

    int size = qMin( option.rect.width(), option.rect.height() );
    QColor c = index.model()->data( index, Qt::DisplayRole ).value<QColor>();

    painter->save();
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setPen( Qt::NoPen );
    painter->setBrush( QBrush( c ) );

    // shorten the rectangle a little to provide some spacing
    QRect shortRect;
    QPoint pt = option.rect.topLeft();
    pt.setX( pt.x() + 1 );
    pt.setY( pt.y() + 1 );
    shortRect.setTopLeft( pt );
    pt = option.rect.bottomRight();
    pt.setX( pt.x() - 1 );
    pt.setY( pt.y() - 1 );
    shortRect.setBottomRight( pt );
    painter->drawRect( shortRect );
    painter->restore();
}

QSize PixelDelegate::sizeHint( const QStyleOptionViewItem & /* option */,
                               const QModelIndex & /* index */ ) const
{
    return QSize( mMonitor->pixelSize(), mMonitor->pixelSize() );
}

bool PixelDelegate::editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index )
{
    if ( !index.isValid() )
        return false;
    switch ( event->type() ) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove: {
        QMouseEvent *mev = static_cast<QMouseEvent*>( event );
        if ( mev->buttons() & Qt::LeftButton ) {
            model->setData( index, mMonitor->currentColor(), Qt::DisplayRole );
            emit imageEdited();
            return true;
        }
    }
    default:
        return false;
    }
}

#include "PixelDelegate.moc"
