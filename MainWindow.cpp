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

MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow )
{
    ui->setupUi( this );
    mModel = new ImageModel;
    ui->mView->setModel( mModel );

    ui->mView->horizontalHeader()->hide();
    ui->mView->verticalHeader()->hide();
    ui->mView->horizontalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setResizeMode( QHeaderView::Fixed );
    ui->mView->horizontalHeader()->setResizeMode( QHeaderView::Fixed );
    ui->mView->horizontalHeader()->setDefaultSectionSize( 12 );
    ui->mView->verticalHeader()->setDefaultSectionSize( 12 );
    ui->mView->viewport()->installEventFilter( this );

    mDelegate = new PixelDelegate( this );
    mDelegate->setPixelSize( 12 );
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

    connect( ui->mSpinBox, SIGNAL( valueChanged( int ) ), mDelegate, SLOT( setPixelSize( int ) ) );
    connect( ui->mSpinBox, SIGNAL( valueChanged( int ) ), SLOT( slotUpdateView( int ) ) );

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDock()
{
    QHBoxLayout *layout = new QHBoxLayout( ui->mColorBox );
    mPalette = new KColorCells( this, 6, 3 );
    mPalette->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    mPalette->setFixedSize( 25*3, 25*6 );

    mPatch = new KColorPatch(this);
    mPatch->setFixedSize( 48, 48 );
    mPatch->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect( mPalette, SIGNAL( colorSelected(int,QColor)), this, SLOT( slotColorSelected(int,QColor)));

    layout->addWidget(mPatch);
    layout->addWidget(mPalette);
    layout->setSpacing(0);
    ui->mColorBox->setLayout( layout );
    setupColors();
}

void MainWindow::setupColors()
{
    if ( !mPalette )
        return;
    mPalette->setColor( 0, QColor("#FFC0C0") );
    mPalette->setColor( 1, QColor("#FF0000") );
    mPalette->setColor( 2, QColor("#C00000") );

    mPalette->setColor( 3, QColor("#FFFFC0") );
    mPalette->setColor( 4, QColor("#FFFF00") );
    mPalette->setColor( 5, QColor("#C0C000") );

    mPalette->setColor( 6, QColor("#C0FFC0") );
    mPalette->setColor( 7, QColor("#00FF00") );
    mPalette->setColor( 8, QColor("#00C000") );

    mPalette->setColor( 9, QColor("#C0FFFF") );
    mPalette->setColor( 10, QColor("#00FFFF") );
    mPalette->setColor( 11, QColor("#00C0C0") );

    mPalette->setColor( 12, QColor("#C0C0FF") );
    mPalette->setColor( 13, QColor("#0000FF") );
    mPalette->setColor( 14, QColor("#0000C0") );

    mPalette->setColor( 15, QColor("#FFC0FF") );
    mPalette->setColor( 16, QColor("#FF00FF") );
    mPalette->setColor( 17, QColor("#C000C0") );
}


bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if ( obj == ui->mView->viewport() && event->type() == QEvent::Wheel ) {
        QWheelEvent * wevent = static_cast<QWheelEvent*>( event );
        // Ctrl + Wheel : change codel size
        if ( wevent->modifiers() == Qt::ControlModifier ) {
            const int numDegrees = wevent->delta() / 8;
            const int numSteps = numDegrees / 15;
            if ( wevent->orientation() == Qt::Vertical ) {
                ui->mSpinBox->setValue( ui->mSpinBox->value() + numSteps );
            }
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::on_actionToggleGrid_triggered()
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

void MainWindow::on_actionOpenSource_triggered()
{

    QString file_name = QFileDialog::getOpenFileName( this, tr( "Open Image File" ),
                        QDesktopServices::storageLocation( QDesktopServices::HomeLocation ),
                        tr( "Images (*.png *.bmp *.ppm *.gif)" ) );
    QImage image( file_name );
    if ( !image.isNull() )
        mModel->setImage( image );

}

void MainWindow::on_actionSaveSource_triggered()
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

    if ( !image.save( file_name, 0 ) )
        QMessageBox::critical( this, tr( "Error saving image" ), tr( "An error occured when trying to save the image." ) );
}

void MainWindow::on_actionNew_triggered()
{
    QImage image( 50, 50, QImage::Format_RGB32 );
    image.fill( QColor( Qt::white ).rgb() );
    mModel->setImage(image);
}


void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionToggleHeaders_triggered()
{
    if ( ui->mView->horizontalHeader()->isVisible() ) { //hide
        ui->mView->horizontalHeader()->hide();
        ui->mView->verticalHeader()->hide();
        ui->mView->horizontalHeader()->setMinimumSectionSize( 1 );
        ui->mView->verticalHeader()->setMinimumSectionSize( 1 );
        ui->mView->verticalHeader()->setResizeMode( QHeaderView::Fixed );
        ui->mView->horizontalHeader()->setResizeMode( QHeaderView::Fixed );
        ui->mView->horizontalHeader()->setDefaultSectionSize( mDelegate->pixelSize() );
        ui->mView->verticalHeader()->setDefaultSectionSize( mDelegate->pixelSize() );
        ui->mSpinBox->setMinimum( 4 );
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
        ui->mSpinBox->setValue( ui->mView->horizontalHeader()->sectionSize( largest_index ) );
        ui->mSpinBox->setMinimum( ui->mView->horizontalHeader()->sectionSize( largest_index ) );
        ui->mView->horizontalHeader()->setMinimumSectionSize( largest_index );
        ui->mView->verticalHeader()->setMinimumSectionSize( largest_index );
    }
}

void MainWindow::slotColorSelected(int index, const QColor& color)
{
    mPatch->setColor( color );
}



#include "MainWindow.moc"
