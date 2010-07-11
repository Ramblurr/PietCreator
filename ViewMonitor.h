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

#ifndef VIEWMONITOR_H
#define VIEWMONITOR_H

#include "Command.h"

#include <QObject>
#include <QColor>
#include <QVector>
#include <QStack>

class KColorCells;

class ViewMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ViewMonitor( QObject * parent );

    QColor currentColor() const;
    int currentColorIndex() const;
    QString currentCommandLabel() const;
    Command currentCommand() const;
    Command takeCommand();

    int pixelSize() const;
    QColor colorForIndex( int index ) const;
    void populateCells( KColorCells * cells );

signals:
    void currentCommandChanged( const Command & newCommand, const Command & oldCommand );
    void pixelSizeChanged( int );
    void currentColorChanged( const QColor & );

public slots:
    void setCurrentCommand( const Command & command );
    void setCurrentColor( int index );
    void setCurrentColor( int index, const QColor& color );
    void setCurrentColor( const QColor& color );

    void setPixelSize( int );

private:
    QStack<Command> mStack;

    int mPixelSize;
    QVector<QColor> mColors;
};

#endif // VIEWMONITOR_H
