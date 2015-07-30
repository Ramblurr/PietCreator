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

#include "CommandDelegate.h"

#include "ViewMonitor.h"

#include "ColorUtils.h"

#include <QtGui>

CommandDelegate::CommandDelegate( ViewMonitor* monitor, QObject* parent ): QStyledItemDelegate( parent ), mMonitor( monitor )
{

}

void CommandDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    Q_ASSERT( index.isValid() );
    QBrush backBrush = index.data( Qt::BackgroundRole ).value<QColor>();
    //QColor foreColor = option.palette.color( QPalette::Text );
    painter->save();
    QStyleOptionViewItemV4 opt( option );
    opt.displayAlignment = Qt::AlignHCenter;
    opt.backgroundBrush = backBrush;
    const QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive( QStyle::PE_PanelItemViewItem, &opt, painter, 0 );

    QColor foreColor(0,0,0);
    if ( ColorUtils::contrastRatio( foreColor, backBrush.color() ) < 5 ) {
        foreColor.setRgb(255,255,255);
    }
    QString label = index.data().toString();
    painter->setPen( foreColor );
    painter->drawText( opt.rect, Qt::AlignCenter, label );
    painter->restore();
}

QSize CommandDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    return QSize( QStyledItemDelegate::sizeHint( option, index ).width(), 25 );
}

