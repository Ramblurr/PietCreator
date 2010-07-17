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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "PixelDelegate.h"
#include "ImageModel.h"
#include "ViewMonitor.h"
#include "ResizeDialog.h"
#include "RunController.h"
#include "OutputModel.h"
#include "CommandWidget.h"
#include "DebugWidget.h"

#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QImage>
#include <QFileDialog>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMessageBox>
#include <QWheelEvent>
#include <QDebug>
#include <QListView>
#include <QKeySequence>
#include <QThread>

static const int INITIAL_CODEL_SIZE = 12;

MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow ),
        mModified( false )
{
    ui->setupUi( this );
    setWindowIcon( QIcon( ":/piet-16x16.png" ) );

    mModel = new ImageModel;
    ui->mView->setModel( mModel );

    ui->mView->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->mView->horizontalHeader()->hide();
    ui->mView->verticalHeader()->hide();
    ui->mView->horizontalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setResizeMode( QHeaderView::Fixed );
    ui->mView->horizontalHeader()->setResizeMode( QHeaderView::Fixed );
    ui->mView->horizontalHeader()->setDefaultSectionSize( 12 );
    ui->mView->verticalHeader()->setDefaultSectionSize( 12 );
    ui->mView->viewport()->setMouseTracking( true ); // to enable Qt::StatusTipRole
    ui->mView->viewport()->installEventFilter( this );

    mMonitor = new ViewMonitor( this );
    mMonitor->setPixelSize( INITIAL_CODEL_SIZE );
    ui->mZoomSlider->setValue( INITIAL_CODEL_SIZE );

    mDelegate = new PixelDelegate( mMonitor, this );
    ui->mView->setItemDelegate( mDelegate );

    mCommandWidget = new CommandWidget( mMonitor, ui->mCommandsPage );
    ui->mCommandsPage->layout()->addWidget( mCommandWidget );

    mDebugWidget = new DebugWidget( ui->mDebugPage );
    ui->mDebugPage->layout()->addWidget( mDebugWidget );
    // setup save message
    mExtensions[ tr( "PNG (*.png)" )] = ".png";
    mExtensions[ tr( "GIF (*.gif)" )] = ".gif";
    mExtensions[ tr( "Portable Pixmap (*.ppm)" )] = ".ppm";

    QHashIterator<QString, QString> it( mExtensions );
    while ( it.hasNext() ) {
        it.next();
        mSaveMessage += it.key();
        if ( it.hasNext() )
            mSaveMessage += ";;";
    }

    connect( mDelegate, SIGNAL( imageEdited() ), SLOT( slotImageEdited() ) );
    connect( ui->mZoomSlider, SIGNAL( valueChanged( int ) ), mMonitor, SLOT( setPixelSize( int ) ) );
    connect( mMonitor, SIGNAL( pixelSizeChanged( int ) ), SLOT( slotUpdateView( int ) ) );

    setupToolbar();

    mOutputModel = new OutputModel( this );
    ui->mOutputView->setModel( mOutputModel );

    mRunController = new RunController;
    connect( mRunController, SIGNAL( newOutput( QString ) ), mOutputModel, SLOT( appendLine( QString ) ) );
    connect( this, SIGNAL( executeSource( QImage ) ), mRunController, SLOT( initializeAndExecute( QImage ) ) );

    mRunController->moveToThread( &mRunThread );
    mRunThread.start();
}

MainWindow::~MainWindow()
{
    mRunThread.quit();
    mRunThread.wait();
    delete ui;
}

void MainWindow::setupToolbar()
{
    QMenu* fileMenu = ui->mMenubar->addMenu( tr( "&File" ) );
    QAction* newAct = ui->mToolBar->addAction( QIcon::fromTheme( "document-new" ), tr( "&New..." ), this, SLOT( slotActionNew() ) );
    newAct->setShortcut( QKeySequence::New );
    fileMenu->addAction( newAct );
    QAction* openAct = ui->mToolBar->addAction( QIcon::fromTheme( "document-open" ), tr( "&Open..." ), this, SLOT( slotActionOpen() ) );
    openAct->setShortcut( QKeySequence::Open );
    fileMenu->addAction( openAct );
    QAction* saveAct = ui->mToolBar->addAction( QIcon::fromTheme( "document-save" ), tr( "&Save" ), this, SLOT( slotActionSave() ) );
    saveAct->setShortcut( QKeySequence::Save );
    saveAct->setDisabled( true );
    connect( this, SIGNAL( validImageDocument( bool ) ), saveAct, SLOT( setEnabled( bool ) ) );
    fileMenu->addAction( saveAct );
    QAction* saveAsAct = ui->mToolBar->addAction( QIcon::fromTheme( "document-save-as" ), tr( "&Save as..." ), this, SLOT( slotActionSaveAs() ) );
    saveAsAct->setShortcut( QKeySequence::SaveAs );
    saveAsAct->setDisabled( true );
    connect( this, SIGNAL( validImageDocument( bool ) ), saveAsAct, SLOT( setEnabled( bool ) ) );
    fileMenu->addAction( saveAsAct );

    fileMenu->addSeparator();

    fileMenu->addAction( QIcon::fromTheme( "application-exit" ), tr( "&Quit" ), this, SLOT( slotActionExit() ) )->setShortcut( QKeySequence::Quit );

    ui->mToolBar->addSeparator();

    QMenu* viewMenu = ui->mMenubar->addMenu( tr( "&View" ) );
    QAction* gridAct = ui->mToolBar->addAction( QIcon::fromTheme( "format-justify-fill" ), tr( "Toggle &Grid" ), this, SLOT( slotActionToggleGrid() ) );
    gridAct->setShortcut( QKeySequence::New );
    gridAct->setDisabled( true );
    connect( this, SIGNAL( validImageDocument( bool ) ), gridAct, SLOT( setEnabled( bool ) ) );
    viewMenu->addAction( gridAct );
    QAction* headersAct = ui->mToolBar->addAction( QIcon::fromTheme( "view-form-table" ), tr( "Toggle &Headers" ), this, SLOT( slotActionToggleHeaders() ) );
    headersAct->setDisabled( true );
    headersAct->setShortcut( QKeySequence::New );
    connect( this, SIGNAL( validImageDocument( bool ) ), headersAct, SLOT( setEnabled( bool ) ) );
    viewMenu->addAction( headersAct );
    QAction* zoomInAct = ui->mToolBar->addAction( QIcon::fromTheme( "zoom-in" ), tr( "Zoom &In" ), this, SLOT( slotActionZoom() ) );
    zoomInAct->setDisabled( true );
    zoomInAct->setData( 1 );
    viewMenu->addAction( zoomInAct );
    connect( this, SIGNAL( validImageDocument( bool ) ), zoomInAct, SLOT( setEnabled( bool ) ) );
    QAction* zoomOutAct = ui->mToolBar->addAction( QIcon::fromTheme( "zoom-out" ), tr( "Zoom &Out" ), this, SLOT( slotActionZoom() ) );
    zoomOutAct->setDisabled( true );
    zoomOutAct->setData( -1 );
    viewMenu->addAction( zoomOutAct );
    connect( this, SIGNAL( validImageDocument( bool ) ), zoomOutAct, SLOT( setEnabled( bool ) ) );
    ui->mToolBar->addSeparator();
    viewMenu->addSeparator();
    QAction* debugViewAct = ui->mToolBar->addAction( QIcon::fromTheme( "utilities-terminal" ), tr( "Toggle Output View" ), this, SLOT( slotToggleOutput() ) );
    viewMenu->addAction( debugViewAct );
    ui->mToolBar->addSeparator();

    QMenu* editMenu = ui->mMenubar->addMenu( tr( "&Edit" ) );
    QAction* resizeAct = ui->mToolBar->addAction( QIcon::fromTheme( "transform-scale" ), tr( "&Resize Image" ), this, SLOT( slotActionResize() ) );
    resizeAct->setDisabled( true );
    connect( this, SIGNAL( validImageDocument( bool ) ), resizeAct, SLOT( setEnabled( bool ) ) );
    editMenu->addAction( resizeAct );
    ui->mToolBar->addSeparator();

    QMenu* progMenu = ui->mMenubar->addMenu( tr( "&Program" ) );
    QAction* runAct = ui->mToolBar->addAction( QIcon::fromTheme( "system-run" ), tr( "&Execute" ), this, SLOT( slotActionRun() ) );
    runAct->setDisabled( true );
    connect( this, SIGNAL( validImageDocument( bool ) ), runAct, SLOT( setEnabled( bool ) ) );
    progMenu->addAction( runAct );
    QAction* debugAct = ui->mToolBar->addAction( QIcon::fromTheme( "run-build" ), tr( "&Debug" ), this, SLOT( slotToggleDebug() ) );
    debugAct->setDisabled( true );
    connect( this, SIGNAL( validImageDocument( bool ) ), debugAct, SLOT( setEnabled( bool ) ) );
    progMenu->addAction( debugAct );
}

void MainWindow::setupDebugPage()
{
   

    
}


void MainWindow::setModified( bool flag )
{
    mModified = flag;
    setWindowModified( flag );
}


bool MainWindow::eventFilter( QObject* obj, QEvent* event )
{
    if ( obj == ui->mView->viewport() && event->type() == QEvent::Wheel ) {
        QWheelEvent * wevent = static_cast<QWheelEvent*>( event );
        // Ctrl + Wheel : change codel size
        if ( wevent->modifiers() == Qt::ControlModifier ) {
            const int numDegrees = wevent->delta() / 8;
            const int numSteps = numDegrees / 15;
            if ( wevent->orientation() == Qt::Vertical ) {
                ui->mZoomSlider->setValue( ui->mZoomSlider->value() + numSteps );
            }
            return true;
        }
    }
}

void MainWindow::slotActionToggleGrid()
{
    ui->mView->setShowGrid( !ui->mView->showGrid() );
}

void MainWindow::slotUpdateView( int pixelSize )
{
    ui->mView->resizeColumnsToContents();
    ui->mView->resizeRowsToContents();
    if ( ui->mView->horizontalHeader()->isVisible() )
        mModel->slotPixelSizeChange( pixelSize );
}

void MainWindow::slotActionOpen()
{
    if ( !promptSave() )
        return;
    QString file_name = QFileDialog::getOpenFileName( this, tr( "Open Image File" ),
                        QDesktopServices::storageLocation( QDesktopServices::HomeLocation ),
                        tr( "Images (*.png *.bmp *.ppm *.gif)" ) );
    QImage image( file_name );
    if ( !image.isNull() )
        mModel->setImage( image );

    setWindowTitle( QString( "Piet Creator - %1 [*]" ).arg( file_name ) );
    setModified( false );
    mCurrentFile = file_name;
    emit validImageDocument( true );
}

void MainWindow::slotActionSaveAs()
{
    QImage image = mModel->image();

    if ( image.isNull() )
        return;

    QString selected_filter;
    QString file_name = QFileDialog::getSaveFileName( this, tr( "Choose a file to save to" ),
                        QDesktopServices::storageLocation( QDesktopServices::HomeLocation ),
                        mSaveMessage,
                        &selected_filter );
    if ( file_name.isEmpty() )
        return;
    QFileInfo file_info( file_name );
    if ( file_info.suffix().isEmpty() )
        file_name.append( mExtensions[selected_filter] );

    if ( !image.save( file_name, 0 ) ) {
        QMessageBox::critical( this, tr( "Error saving image" ), tr( "An error occured when trying to save the image." ) );
    } else {
        setWindowTitle( QString( "Piet Creator - %1 [*]" ).arg( file_name ) );
        setModified( false );
        mCurrentFile = file_name;
    }
}

void MainWindow::slotActionSave()
{
    if ( mCurrentFile.isEmpty() )
        return slotActionSaveAs();

    QImage image = mModel->image();

    if ( image.isNull() )
        return;

    QString selected_filter;
    QString file_name = mCurrentFile.toLocalFile();
    QFileInfo file_info( file_name );
    if ( !file_info.isWritable() )
        return slotActionSaveAs();

    if ( file_info.suffix().isEmpty() )
        file_name.append( mExtensions[selected_filter] );

    if ( !image.save( file_name, 0 ) ) {
        QMessageBox::critical( this, tr( "Error saving image" ), tr( "An error occured when trying to save the image." ) );
    } else {
        setWindowTitle( QString( "Piet Creator - %1 [*]" ).arg( file_name ) );
        setModified( false );
        mCurrentFile = file_name;
    }
}


void MainWindow::slotActionNew()
{
    if ( !promptSave() )
        return;
    QImage image( 50, 50, QImage::Format_RGB32 );
    image.fill( QColor( Qt::white ).rgb() );
    mModel->setImage( image, 1 );
    setWindowTitle( "Piet Creator - new source [*]" );
    setModified( false );
    mCurrentFile.clear();
    emit validImageDocument( true );
}


void MainWindow::slotActionExit()
{
    if ( !promptSave() )
        return;
    qApp->quit();
}

void MainWindow::slotActionToggleHeaders()
{
    if ( ui->mView->horizontalHeader()->isVisible() ) { //hide
        ui->mView->horizontalHeader()->hide();
        ui->mView->verticalHeader()->hide();
        ui->mView->horizontalHeader()->setMinimumSectionSize( 1 );
        ui->mView->verticalHeader()->setMinimumSectionSize( 1 );
        ui->mView->verticalHeader()->setResizeMode( QHeaderView::Fixed );
        ui->mView->horizontalHeader()->setResizeMode( QHeaderView::Fixed );
        ui->mView->horizontalHeader()->setDefaultSectionSize( mMonitor->pixelSize() );
        ui->mView->verticalHeader()->setDefaultSectionSize( mMonitor->pixelSize() );
        ui->mZoomSlider->setMinimum( 4 );
        mModel->slotPixelSizeChange( 1 );
    }  else { //show
        ui->mView->horizontalHeader()->show();
        ui->mView->verticalHeader()->show();
        ui->mView->verticalHeader()->setResizeMode( QHeaderView::Fixed );
        ui->mView->horizontalHeader()->setResizeMode( QHeaderView::Fixed );

        int rows = mModel->rowCount();
        if ( rows == 0 )
            return;
        int num_digits;
        while ( rows > 0 ) {
            ++num_digits;
            rows /= 10;
        }
        int largest_index = num_digits * 10; // not the largest index, but one of the largest (most digits)
        ui->mZoomSlider->setValue( ui->mView->horizontalHeader()->sectionSize( largest_index ) );
        ui->mZoomSlider->setMinimum( ui->mView->horizontalHeader()->sectionSize( largest_index ) );
        ui->mView->horizontalHeader()->setMinimumSectionSize( largest_index );
        ui->mView->verticalHeader()->setMinimumSectionSize( largest_index );
        slotUpdateView( mMonitor->pixelSize() );
    }
}

void MainWindow::slotActionResize()
{
    QScopedPointer<ResizeDialog> dlg( new ResizeDialog( mModel->imageSize(), this ) );
    if ( dlg->exec() == QDialog::Accepted ) {
        QSize size = dlg->newSize();
        if ( size.isValid() ) {
            if ( size.width() < mModel->imageSize().width() || size.height() < mModel->imageSize().height() ) {
                int but = QMessageBox::warning( this,
                                                tr( "Resize Image" ),
                                                tr( "The new image dimensions are smaller than the current dimensions. Data loss will occur. Apply the resize operation or Abort?" ),
                                                QMessageBox::Apply  | QMessageBox::Abort, QMessageBox::Abort );
                if ( but == QMessageBox::Abort )
                    return;
            }
            mModel->scaleImage( size );
        }
    }
}

void MainWindow::slotActionZoom()
{
    QAction* act = qobject_cast< QAction* >( sender() );
    ui->mZoomSlider->setValue( ui->mZoomSlider->value() +  act->data().toInt() );
}

void MainWindow::slotActionDebug()
{
    qDebug() << "debug clicked";
}

void MainWindow::slotActionRun()
{
    ui->mOutputView->setVisible( true );
    emit executeSource( mModel->image() );
}

void MainWindow::slotImageEdited()
{
    if ( !mModified )
        setModified( true );
}

bool MainWindow::promptSave()
{
    if ( mModified ) {
        int but = QMessageBox::warning( this,
                                        tr( "Modifed" ),
                                        tr( "The current source image has been modified. Opening a new image will discard the current changes." ),
                                        QMessageBox::Discard  | QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save );
        switch ( but ) {
        case QMessageBox::Save:
            slotActionSaveAs();
            return true;
        case QMessageBox::Cancel:
            return false;
        case QMessageBox::Discard:
        default:
            return true;
        }
    }
    return true;
}

void MainWindow::slotToggleOutput()
{
    if( ui->dockWidget_2->isVisible() ) { //hide
        ui->dockWidget_2->hide();
    } else { //show
        ui->dockWidget_2->show();
    }
}

void MainWindow::slotToggleDebug()
{
    ui->mStackedWidget->setCurrentIndex( !ui->mStackedWidget->currentIndex()  );
}

#include "MainWindow.moc"
