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

#ifndef PIXELDELEGATE_H
#define PIXELDELEGATE_H

#include <QAbstractItemDelegate>

class ViewMonitor;
class QMenu;

class PixelDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    PixelDelegate( ViewMonitor* monitor, QMenu* menu, QObject *parent = 0 );
    virtual ~PixelDelegate() {}

    void paint( QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index ) const;

    QSize sizeHint( const QStyleOptionViewItem &option,
                    const QModelIndex &index ) const;

    bool editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index );

signals:
    void imageEdited();

private:
    ViewMonitor* mMonitor;
    QMenu* mContextMenu;
};

#endif