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

#include "CommandWidget.h"

#include "KColorCells.h"
#include "KColorPatch.h"
#include "ViewMonitor.h"
#include "CommandsModel.h"
#include "CommandDelegate.h"
#include "Command.h"

#include <QBoxLayout>
#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QDebug>

CommandWidget::CommandWidget( ViewMonitor* monitor, QWidget* parent ): QWidget( parent ), mMonitor( monitor )
{
    QBoxLayout *mainLayout = new QVBoxLayout( this );
    setLayout( mainLayout );

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    QWidget *colorsWidget = new QWidget( this );
    QHBoxLayout *hlayout = new QHBoxLayout( colorsWidget );
    mPalette = new KColorCells( colorsWidget, 6, 3 );
    mPalette->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    mPalette->setFixedSize( 25 * 3, 25 * 6 );
    mMonitor->populateCells( mPalette );

    KColorCells* mBWPalette = new KColorCells( colorsWidget, 2, 1 );
    mBWPalette->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    mBWPalette->setFixedSize( 25, 25 * 6 );
    mBWPalette->setColor( 0, Qt::black );
    mBWPalette->setColor( 1, Qt::white );

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

    hlayout->setSpacing( 0 );
    hlayout->addStretch();
    hlayout->addWidget( currentCommandWidget );
    hlayout->addWidget( mPalette );
    hlayout->addWidget( mBWPalette );
    hlayout->addStretch();
    colorsWidget->setLayout( hlayout );

    mCommandsModel = new CommandsModel( mMonitor, this );
    mCommandsView = new QTableView( this );
    mCommandsView->horizontalHeader()->hide();
    mCommandsView->verticalHeader()->hide();
    mCommandsView->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
    mCommandsView->setModel( mCommandsModel );
    mCommandDelegate = new CommandDelegate( mMonitor, this );
    mCommandsView->setItemDelegate( mCommandDelegate );

    mainLayout->insertWidget( 0, colorsWidget );
    mainLayout->insertWidget( 1, mCommandsView );
    mainLayout->addStretch();

    connect( mPalette, SIGNAL( colorSelected( int, QColor ) ), mMonitor, SLOT( setCurrentColor( int, QColor ) ) );
    connect( mBWPalette, SIGNAL( colorSelected( int, QColor ) ), mMonitor, SLOT( setCurrentColor( int, QColor ) ) );
    connect( mMonitor, SIGNAL( currentCommandChanged( Command, Command ) ), mCommandsView, SLOT( reset() ) );
    connect( mMonitor, SIGNAL( currentColorChanged( QColor ) ), mCommandsView, SLOT( reset() ) );
    connect( mMonitor, SIGNAL( currentColorChanged( QColor ) ), mPrimaryPatch, SLOT( setColor( QColor ) ) );
    connect( mMonitor, SIGNAL( currentCommandChanged( Command, Command ) ), this, SLOT( slotCurrentCommandChanged( Command, Command ) ) );
    connect( mCommandsView, SIGNAL( clicked( QModelIndex ) ), this, SLOT( slotCommandClicked( QModelIndex ) ) );
    connect( mCommandsView->horizontalHeader(), SIGNAL( geometriesChanged() ), this, SLOT( slotSetViewWidth() ) );

    Command firstcmd = mCommandsModel->data( mCommandsModel->index( 0, 0 ), CommandsModel::CommandRole ).value<Command>();
    mMonitor->setCurrentCommand( firstcmd );
    slotSetViewWidth();
}

CommandWidget::~CommandWidget()
{

}

void CommandWidget::slotCurrentCommandChanged( const Command& newCommand, const Command& oldCommand )
{
    mPrimaryPatch->setColor( newCommand.color );
    mPrimaryCommandLabel->setText( newCommand.name );
    mSecondaryPatch->setColor( oldCommand.color );
    mSecondaryCommandLabel->setText( oldCommand.name );
}

void CommandWidget::slotCommandClicked( const QModelIndex& index )
{
    Command command = index.data( CommandsModel::CommandRole ).value<Command>();
    qDebug() << "command clicked:" << command.name << command.index;
    mMonitor->setCurrentCommand( command );
}

bool CommandWidget::eventFilter( QObject* obj, QEvent* event )
{
    if ( obj == mSecondaryPatch && event->type() == QEvent::MouseButtonRelease ) {
        QMouseEvent * mevent = static_cast<QMouseEvent*>( event );
        if ( mevent->button() == Qt::LeftButton ) {
            mMonitor->takeCommand();
            return true;
        }
    }
    return QObject::eventFilter( obj, event );
}

void CommandWidget::slotSetViewWidth()
{
    // This table will always have 3 columns.
    // if this changes we have bigger worries
    mCommandsView->resizeColumnsToContents();
    int width = 0;
    width += mCommandsView->columnWidth( 0 );
    width += mCommandsView->columnWidth( 1 );
    width += mCommandsView->columnWidth( 2 );
    width += mCommandsView->frameWidth();
    width += 3;

    mCommandsView->setMinimumWidth( width );
    mCommandsView->adjustSize();
    adjustSize();
}


#include "CommandWidget.moc"
