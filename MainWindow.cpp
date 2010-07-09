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

#include <QTableView>
#include <QHeaderView>
#include <QImage>
#include <QFileDialog>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMessageBox>

MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow )
{
    ui->setupUi( this );
    mModel = new ImageModel;

    ui->mView->horizontalHeader()->hide();
    ui->mView->verticalHeader()->hide();
    ui->mView->horizontalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setResizeMode( QHeaderView::Fixed );
    ui->mView->horizontalHeader()->setResizeMode( QHeaderView::Fixed );
    ui->mView->horizontalHeader()->setDefaultSectionSize( 12 );
    ui->mView->verticalHeader()->setDefaultSectionSize( 12 );
    ui->mView->setModel( mModel );


    PixelDelegate* delegate = new PixelDelegate( this );
    delegate->setPixelSize( 12 );
    ui->mView->setItemDelegate( delegate );

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

    connect( ui->mSpinBox, SIGNAL( valueChanged( int ) ), delegate, SLOT( setPixelSize( int ) ) );
    connect( ui->mSpinBox, SIGNAL( valueChanged( int ) ), SLOT( slotUpdateView() ) );

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionToggleGrid_triggered()
{
    mView->setShowGrid( !mView->showGrid() );
}

void MainWindow::slotUpdateView()
{
    ui->mView->resizeColumnsToContents();
    ui->mView->resizeRowsToContents();
}


#include "MainWindow.moc"

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

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}
