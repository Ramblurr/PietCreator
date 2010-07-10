/*
    Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
    This file was part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)

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

#ifndef KCOLORPATCH_H
#define KCOLORPATCH_H

#include <QFrame>

class QMouseEvent;
class QDragEnterEvent;
class QDropEvent;

/**
 * @short A color displayer.
 *
 * The KColorPatch widget is a (usually small) widget showing
 * a selected color e.g. in the KColorDialog. It
 * automatically handles drag and drop from and on the widget.
 *
 * \image html kcolorpatch.png "KDE Color Patch"
 */
class KColorPatch : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    KColorPatch( QWidget *parent );
    virtual ~KColorPatch();

    /**
     * Get the currently displayed color
     */
    QColor color() const;

public slots:
    /**
     * Set the color to display and update the display
     *
     * @param col color to display
     */
    void setColor( const QColor &col );

Q_SIGNALS:
    /**
     * This signal is emitted whenever the current color
     * changes due to a drop event
     */
    void colorChanged( const QColor& newColor, const QColor& oldColor );

protected:
    virtual void paintEvent    ( QPaintEvent * pe );
    virtual void mouseMoveEvent( QMouseEvent * );
    virtual void dragEnterEvent( QDragEnterEvent *);
    virtual void dropEvent( QDropEvent *);

private:
    class KColorPatchPrivate;
    KColorPatchPrivate *const d;

    Q_DISABLE_COPY(KColorPatch)
};

#endif //KCOLORPATCH_H
