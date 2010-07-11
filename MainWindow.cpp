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
#include "KColorCells.h"
#include "KColorPatch.h"
#include "ViewMonitor.h"
#include "CommandsModel.h"
#include "CommandDelegate.h"
#include "Command.h"
#include "ResizeDialog.h"

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
    setupDock();

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
}

MainWindow::~MainWindow()
{
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

    QMenu* editMenu = ui->mMenubar->addMenu( tr( "&Edit" ) );
    QAction* resizeAct = ui->mToolBar->addAction( QIcon::fromTheme( "format-justify-fill" ), tr( "&Resize Image" ), this, SLOT( slotActionResize() ) );
    resizeAct->setDisabled( true );
    connect( this, SIGNAL( validImageDocument( bool ) ), resizeAct, SLOT( setEnabled( bool ) ) );
    editMenu->addAction( resizeAct );
}

void MainWindow::setupDock()
{
    QWidget *colorsWidget = new QWidget( ui->mDockContents );
    QHBoxLayout *hlayout = new QHBoxLayout( colorsWidget );
    mPalette = new KColorCells( colorsWidget, 6, 3 );
    mPalette->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    mPalette->setFixedSize( 25*3, 25*6 );
    mMonitor->populateCells( mPalette );

    const int spacing = 15;
    QWidget *currentCommandWidget = new QWidget( colorsWidget );
    QVBoxLayout *vlayout = new QVBoxLayout( currentCommandWidget );
    QWidget *patchWidget = new QWidget( currentCommandWidget );
    patchWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    patchWidget->setFixedWidth( 48 + spacing );
    mSecondaryPatch = new KColorPatch( patchWidget );
    mSecondaryPatch->setFixedSize( 48, 48 );
    mSecondaryPatch->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    mSecondaryPatch->installEventFilter( this );

    mPrimaryPatch = new KColorPatch( patchWidget );
    mPrimaryPatch->setFixedSize( 48, 48 );
    mPrimaryPatch->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    QRect secRect = mPrimaryPatch->geometry();
    secRect.setTopLeft( QPoint( secRect.topLeft().x() + spacing,  secRect.topLeft().y() + spacing ) );
    mSecondaryPatch->setGeometry( secRect );

    mPrimaryCommandLabel = new QLabel( currentCommandWidget );
    mPrimaryCommandLabel->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    mSecondaryCommandLabel = new QLabel( currentCommandWidget );
    mSecondaryCommandLabel->setAlignment( Qt::AlignRight );
    mSecondaryCommandLabel->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    patchWidget->setFixedHeight( 48 + spacing );
    vlayout->addWidget( mPrimaryCommandLabel );
    vlayout->addWidget( patchWidget );
    vlayout->addWidget( mSecondaryCommandLabel );
    vlayout->addStretch();
    currentCommandWidget->setLayout( vlayout );

    hlayout->addStretch();
    hlayout->addWidget( currentCommandWidget );
    hlayout->addWidget( mPalette );
    hlayout->addStretch();
    colorsWidget->setLayout( hlayout );

//     QWidget *commandsWidget = new QWidget( ui->mDockContents );
    mCommandsModel = new CommandsModel( mMonitor, this );
    QTableView *commandsView = new QTableView( ui->mDockContents );
    commandsView->horizontalHeader()->hide();
    commandsView->verticalHeader()->hide();
//     commandsView->horizontalHeader()->setMinimumSectionSize( 1 );
//     commandsView->verticalHeader()->setMinimumSectionSize( 1 );
//     commandsView->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents );
    commandsView->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
//     commandsView->horizontalHeader()->setDefaultSectionSize( 50 );
//     commandsView->verticalHeader()->setDefaultSectionSize( 50 );
    commandsView->setModel( mCommandsModel );

    mCommandDelegate = new CommandDelegate( mMonitor, this );
    commandsView->setItemDelegate( mCommandDelegate );

    QBoxLayout *boxLayout = static_cast<QBoxLayout*>( ui->mDockContents->layout() );
    boxLayout->insertWidget( 0, colorsWidget );
    boxLayout->insertWidget( 1, commandsView );

    connect( mPalette, SIGNAL( colorSelected( int, QColor ) ), mMonitor, SLOT( setCurrentColor( int, QColor ) ) );
    connect( mMonitor, SIGNAL( currentCommandChanged( Command, Command ) ), commandsView, SLOT( reset() ) );
    connect( mMonitor, SIGNAL( currentColorChanged( QColor ) ), commandsView, SLOT( reset() ) );
    connect( mMonitor, SIGNAL( currentCommandChanged( Command, Command ) ), this, SLOT( slotCurrentCommandChanged( Command, Command ) ) );
    connect( commandsView, SIGNAL( clicked( QModelIndex ) ), this, SLOT( slotCommandClicked( QModelIndex ) ) );
//     connect( mPrimaryPatch, SIGNAL( colorChanged(QColor,QColor)), this, SLOT( slotHandlePatchChange( QColor, QColor ) ) );

    // select default
    Command firstcmd = mCommandsModel->data( mCommandsModel->index( 0, 0 ), CommandsModel::CommandRole ).value<Command>();
    mMonitor->setCurrentCommand( firstcmd );
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
    if ( obj == mSecondaryPatch && event->type() == QEvent::MouseButtonRelease ) {
        QMouseEvent * mevent = static_cast<QMouseEvent*>( event );
        if ( mevent->button() == Qt::LeftButton ) {
            mMonitor->takeCommand();
            return true;
        }

    }
    return QObject::eventFilter( obj, event );
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


void MainWindow::slotCommandClicked( const QModelIndex &index )
{
    Command command = index.data( CommandsModel::CommandRole ).value<Command>();
    qDebug() << "command clicked:" << command.name << command.index;
    mMonitor->setCurrentCommand( command );
}

void MainWindow::slotCurrentCommandChanged( const Command& newCommand, const Command& oldCommand )
{
    mPrimaryPatch->setColor( newCommand.color );
    mPrimaryCommandLabel->setText( newCommand.name );
    mSecondaryPatch->setColor( oldCommand.color );
    mSecondaryCommandLabel->setText( oldCommand.name );
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


#include "MainWindow.moc"
