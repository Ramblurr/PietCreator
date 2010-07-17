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

#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <QWidget>
#include <QModelIndex>

class CommandDelegate;
class CommandsModel;
class KColorCells;
class KColorPatch;
class QLabel;
class Command;
class ViewMonitor;
class QEvent;

class CommandWidget : public QWidget
{
    Q_OBJECT
public:
    CommandWidget( ViewMonitor* monitor, QWidget* parent = 0 );
    virtual ~CommandWidget();

    virtual bool eventFilter( QObject* obj, QEvent* event );

public slots:
    void slotCurrentCommandChanged( const Command & newCommand, const Command & oldCommand );

private slots:
    void slotCommandClicked( const QModelIndex &index );
private:
    CommandsModel *mCommandsModel;
    CommandDelegate* mCommandDelegate;
    KColorCells* mPalette;
    KColorPatch* mPrimaryPatch;
    KColorPatch* mSecondaryPatch;
    QLabel *mPrimaryCommandLabel;
    QLabel *mSecondaryCommandLabel;
    ViewMonitor *mMonitor;

};

#endif // COMMANDWIDGET_H
