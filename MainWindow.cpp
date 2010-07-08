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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mModel = new ImageModel;

    ui->mView->horizontalHeader()->hide();
    ui->mView->verticalHeader()->hide();
    ui->mView->horizontalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setMinimumSectionSize( 1 );
    ui->mView->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->mView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->mView->horizontalHeader()->setDefaultSectionSize( 12 );
    ui->mView->verticalHeader()->setDefaultSectionSize( 12 );
    ui->mView->setModel( mModel );


    asdgadsfh

    PixelDelegate* delegate = new PixelDelegate( this );
    delegate->setPixelSize( 12 );
    ui->mView->setItemDelegate( delegate );
    mModel->setImage( QImage("../nhello.ppm") );

    connect( ui->mSpinBox, SIGNAL( valueChanged(int)), delegate, SLOT( setPixelSize(int)));
    connect( ui->mSpinBox, SIGNAL( valueChanged(int)), SLOT( slotUpdateView()));

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