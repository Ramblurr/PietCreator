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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

namespace Ui
{
class MainWindow;
}

class PixelDelegate;
class CommandDelegate;
class ImageModel;
class KColorCells;
class KColorPatch;
class ViewMonitor;
class QTableView;
class QModelIndex;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

    virtual bool eventFilter(QObject* , QEvent* );

private slots:
    void on_actionExit_triggered();
    void on_actionSaveSource_triggered();
    void on_actionOpenSource_triggered();
    void on_actionToggleGrid_triggered();
    void on_actionToggleHeaders_triggered();
    void on_actionNew_triggered();
    void slotUpdateView( int pixelSize );
    void slotCommandClicked( const QModelIndex &index );
    void slotHandlePatchChange( const QColor &newColor, const QColor &oldColor );

private:
    void setupDock();

    Ui::MainWindow *ui;

    QString mSaveMessage;
    QHash<QString, QString> mExtensions;

    ImageModel* mModel;
    PixelDelegate* mDelegate;
    CommandDelegate* mCommandDelegate;
    KColorCells* mPalette;
    KColorPatch* mPrimaryPatch;
    KColorPatch* mSecondaryPatch;
    QLabel *mPrimaryCommandLabel;
    QLabel *mSecondaryCommandLabel;
    ViewMonitor* mMonitor;
};

#endif // MAINWINDOW_H
