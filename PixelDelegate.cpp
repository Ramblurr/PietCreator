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

#include "ImageModel.h"
#include "ViewMonitor.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

PixelDelegate::PixelDelegate( ViewMonitor* monitor, QObject *parent ) : QAbstractItemDelegate( parent ), mMonitor( monitor )
{
}

void PixelDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{

//     if ( option.state & QStyle::State_Selected )
//         painter->fillRect( option.rect, option.palette.highlight() );

    int size = qMin( option.rect.width(), option.rect.height() );
    QColor c = index.model()->data( index, Qt::DisplayRole ).value<QColor>();

    painter->save();
    painter->setPen( Qt::NoPen );
    painter->setBrush( QBrush( c ) );

    // shorten the rectangle a little to provide some spacing
    QRect shortRect = option.rect.adjusted( 1, 1, -1, -1 );
    painter->drawRect( shortRect );
    painter->restore();

    painter->save();
    QPen pen = painter->pen();
    if( index.data( ImageModel::IsCurrentDebugRole ).toBool() ) {
        pen.setColor( Qt::gray );
        pen.setWidth( 2 );
        painter->setPen( pen );
        painter->drawRect( shortRect );
    } else {
        pen.setColor( Qt::gray );
        pen.setWidth( 1 );
        painter->setPen( pen );
        painter->drawRect( shortRect );
    }
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
        } else if ( mev->buttons() & Qt::RightButton ) {
            mMonitor->setCurrentColor(  index.model()->data( index, Qt::DisplayRole ).value<QColor>() );
            return false;
        }
    }
    default:
        return false;
    }
}

#include "PixelDelegate.moc"
