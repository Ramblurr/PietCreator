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
#include <QUrl>
#include <QThread>

namespace Ui
{
class MainWindow;
}

class PixelDelegate;
class ImageModel;
class ViewMonitor;
class QTableView;
class OutputModel;
class RunController;
class CommandWidget;
class DebugWidget;
class UndoHandler;
class QUndoStack;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

    virtual bool eventFilter( QObject* , QEvent* );
signals:
    void validImageDocument( bool );
    void executeSource( const QImage & );
    void debugSource( const QImage & );
    void debugStep();
    void debugStop();
    void debugStarted( bool );
    void setStopEnabled( bool );

private slots:
    void slotActionExit();
    void slotActionSaveAs();
    void slotActionSave();
    void slotActionOpen();
    void slotActionToggleGrid();
    void slotActionToggleHeaders();
    void slotActionNew();
    void slotActionResize();
    void slotActionInsert();
    void slotActionZoom();
    void slotActionDebug();
    void slotActionRun();

    void slotUpdateView( int pixelSize );
    void slotImageEdited();

    void slotToggleOutput();
    void slotClearOutputView();
    void slotStartDebug();

    void slotControllerStopped();
    void slotControllerStarted();
    void slotGetChar();
    void slotGetInt();
    void slotReturnPressed();

    void slotStopController();

    void slotNewOutput( QString );

private:
    void setupToolbar();
    void setModified( bool flag );
    bool promptSave();

    Ui::MainWindow *ui;

    QString mSaveMessage;
    QHash<QString, QString> mExtensions;

    QUndoStack* mUndoStack;
    UndoHandler* mUndoHandler;
    ImageModel* mModel;
    PixelDelegate* mDelegate;
    ViewMonitor* mMonitor;
    OutputModel* mOutputModel;
    RunController* mRunController;
    CommandWidget* mCommandWidget;
    DebugWidget* mDebugWidget;

    QThread mRunThread;
    QUrl mCurrentFile;
    bool mModified;
    bool mWaitInt;
    bool mWaitChar;
    bool mWaitingForCoordSelection;
    QImage mInsertImage;
};

#endif // MAINWINDOW_H
