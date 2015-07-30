/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 2007 Roberto Raggi (roberto@kdevelop.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "KColorCells.h"
#include "KColorMimeData.h"
#include "ColorUtils.h"

#include <QtGui>

#include <math.h>

class KColorCells::KColorCellsPrivate
{
public:
    KColorCellsPrivate( KColorCells *q ): q( q ) {
        inMouse = false;
        selected = -1;
        shade = false;
    }

    KColorCells *q;
    QPoint mousePos;
    int selected;
    bool shade;
    bool inMouse;
};

class KColorCellsItemDelegate: public QStyledItemDelegate
{
public:
    KColorCellsItemDelegate( KColorCells *parent ): QStyledItemDelegate( parent ) {}
    virtual void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const {
        QStyleOptionViewItemV4 opt( option );
        initStyleOption( &opt, index );

        //Get the current cell color
        QColor backgroundColor = index.data( Qt::BackgroundRole ).value<QColor>();
        if ( backgroundColor.isValid() ) {
            //Paint the general background
            painter->fillRect( opt.rect, backgroundColor );
            //Paint the selection mark (circle)
            if ( opt.state & QStyle::State_Selected ) {
                //Use black or white, depending on the contrast
                QColor color = QColor( 0, 0, 0, 220 );
                if ( ColorUtils::contrastRatio( color, backgroundColor ) < 5 ) {
                    color = QColor( 255, 255, 255, 220 );
                }
                //Draw the selection (radiobutton-like) circle
                painter->save();
                painter->setRenderHint( QPainter::Antialiasing, true );
                painter->setRenderHint( QPainter::HighQualityAntialiasing, true );
                painter->setPen( QPen( color, 1.2, Qt::SolidLine ) );
                painter->setBrush( QBrush() );
                painter->drawEllipse( opt.rect.adjusted( 2, 2, -2, -2 ) );
                painter->restore();
            }
        } else {
            //Paint the "X" (missing) cross on empty background color
            backgroundColor = opt.palette.color( QPalette::Window );
            painter->fillRect( opt.rect, backgroundColor );
            painter->save();
            QColor crossColor = qGray( backgroundColor.rgb() ) > 192 ? backgroundColor.darker( 106 ) :
                                backgroundColor.lighter( 106 );
            painter->setPen( QPen( crossColor, 1.5 ) );
            painter->drawLine( opt.rect.topLeft(), opt.rect.bottomRight() );
            painter->drawLine( opt.rect.topRight(), opt.rect.bottomLeft() );
            painter->restore();
        }
    }
};

KColorCells::KColorCells( QWidget *parent, int rows, int cols )
    : QTableWidget( parent ), d( new KColorCellsPrivate( this ) )
{
    setItemDelegate( new KColorCellsItemDelegate( this ) );

    setFrameShape( QFrame::NoFrame );
    d->shade = true;
    setRowCount( rows );
    setColumnCount( cols );

    verticalHeader()->hide();
    horizontalHeader()->hide();

    d->selected = 0;
    d->inMouse = false;

    // Drag'n'Drop
    setAcceptDrops( true );

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    viewport()->setBackgroundRole( QPalette::Background );
    setBackgroundRole( QPalette::Background );


    setSelectionMode( QAbstractItemView::SingleSelection );
    setDragEnabled( false );
}

KColorCells::~KColorCells()
{
    delete d;
}

QColor KColorCells::color( int index ) const
{
    QTableWidgetItem * tmpItem = item( index / columnCount(), index % columnCount() );

    if ( tmpItem != 0 )
        return tmpItem->data( Qt::BackgroundRole ).value<QColor>();

    return QColor();
}

int KColorCells::count() const
{
    return rowCount() * columnCount();
}

void KColorCells::setShading( bool _shade )
{
    d->shade = _shade;
}

void KColorCells::setAcceptDrags( bool _acceptDrags )
{
    this->setDragEnabled( _acceptDrags );
}

void KColorCells::setSelected( int index )
{
    Q_ASSERT( index >= 0 && index < count() );

    d->selected = index;
}

int KColorCells::selectedIndex() const
{
    return d->selected;
}

void KColorCells::setColor( int column, const QColor &color )
{
    const int tableRow = column / columnCount();
    const int tableColumn = column % columnCount();

    Q_ASSERT( tableRow >= 0 && tableRow < rowCount() );
    Q_ASSERT( tableColumn >= 0 && tableColumn < columnCount() );

    QTableWidgetItem * tableItem = item( tableRow, tableColumn );

    if ( tableItem == 0 ) {
        tableItem = new QTableWidgetItem();
        setItem( tableRow, tableColumn, tableItem );
    }

    tableItem->setData( Qt::BackgroundRole , color );
}

void KColorCells::resizeEvent( QResizeEvent* )
{
    // According to the Qt doc:
    //   If you need to set the width of a given column to a fixed value, call
    //   QHeaderView::resizeSection() on the table's {horizontal,vertical}
    //   header.
    // Therefore we iterate over each row and column and set the header section
    // size, as the sizeHint does indeed appear to be ignored in favor of a
    // minimum size that is larger than what we want.
    for ( int index = 0 ; index < columnCount() ; index++ )
        horizontalHeader()->resizeSection( index, sizeHintForColumn( index ) );
    for ( int index = 0 ; index < rowCount() ; index++ )
        verticalHeader()->resizeSection( index, sizeHintForRow( index ) );
}

int KColorCells::sizeHintForColumn( int /*column*/ ) const
{
    return width() / columnCount() ;
}

int KColorCells::sizeHintForRow( int /*row*/ ) const
{
    return height() / rowCount() ;
}

void KColorCells::mousePressEvent( QMouseEvent *e )
{
    d->inMouse = true;
    d->mousePos = e->pos();

    QTableWidget::mousePressEvent( e );
}


int KColorCells::positionToCell( const QPoint &pos, bool ignoreBorders ) const
{
    //TODO ignoreBorders not yet handled
    Q_UNUSED( ignoreBorders )

    QTableWidgetItem* tableItem = itemAt( pos );

    if ( !tableItem )
        return -1;

    const int itemRow = row( tableItem );
    const int itemColumn = column( tableItem );
    int cell = itemRow * columnCount() + itemColumn;

    /*if (!ignoreBorders)
    {
       int border = 2;
       int x = pos.x() - col * cellWidth();
       int y = pos.y() - row * cellHeight();
       if ( (x < border) || (x > cellWidth()-border) ||
            (y < border) || (y > cellHeight()-border))
          return -1;
    }*/

    return cell;
}

void KColorCells::mouseMoveEvent( QMouseEvent *e )
{
    if ( this->dragEnabled() || this->acceptDrops() ) {
        if ( !( e->buttons() & Qt::LeftButton ) ) return;

        if ( d->inMouse ) {
            if ( e->x() > d->mousePos.x()  || e->x() < d->mousePos.x() ||
                 e->y() > d->mousePos.y()  || e->y() < d->mousePos.y() ) {
                // Drag color object
                QTableWidgetItem * tableItem = itemAt( d->mousePos );

                if ( tableItem ) {
                    QVariant var = tableItem->data( Qt::BackgroundRole );
                    QColor tmpCol = var.value<QColor>();
                    if ( tmpCol.isValid() )
                        KColorMimeData::createDrag( tmpCol, this )->start();
                }
            }
        }
    } else
        QTableWidget::mouseMoveEvent( e );
}

void KColorCells::dragEnterEvent( QDragEnterEvent *event )
{
    qDebug() << "KColorCells::dragEnterEvent() acceptDrags="
             << this->dragEnabled()
             << " canDecode=" << KColorMimeData::canDecode( event->mimeData() )
             << endl;
    event->setAccepted( this->dragEnabled() && KColorMimeData::canDecode( event->mimeData() ) );
}

// Reimplemented to override QTableWidget's override.  Else dropping doesn't work.
void KColorCells::dragMoveEvent( QDragMoveEvent *event )
{
    qDebug() << "KColorCells::dragMoveEvent() acceptDrags="
             << this->dragEnabled()
             << " canDecode=" << KColorMimeData::canDecode( event->mimeData() )
             << endl;
    event->setAccepted( this->dragEnabled() && KColorMimeData::canDecode( event->mimeData() ) );
}

void KColorCells::dropEvent( QDropEvent *event )
{
    QColor c = KColorMimeData::fromMimeData( event->mimeData() );

    qDebug() << "KColorCells::dropEvent() color.isValid=" << c.isValid();
    if ( c.isValid() ) {
        QTableWidgetItem * tableItem = itemAt( event->pos() );

        if ( tableItem )
            tableItem->setData( Qt::BackgroundRole , c );
    }
}

void KColorCells::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
QModelIndexList indexList = selected.indexes();
foreach (QModelIndex index, indexList) {
    int row = index.row();
    int column = index.column();
    int cell = row * columnCount() + column;
    emit colorSelected( cell , color( cell ) );
    break;
}

    QTableView::selectionChanged(selected, deselected);
}

void KColorCells::mouseReleaseEvent( QMouseEvent *e )
{
    d->inMouse = false;

    QTableWidget::mouseReleaseEvent( e );
}

void KColorCells::mouseDoubleClickEvent( QMouseEvent * /*e*/ )
{
    int cell = positionToCell( d->mousePos );

    if ( cell != -1 )
        emit colorDoubleClicked( cell , color( cell ) );
}

#include "KColorCells.moc"
