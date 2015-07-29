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

#include "ImageModel.h"
extern "C"
{
#include "npiet.h"
}

#include <QtGui>
#include <QMessageBox>
#include <QDebug>

ImageModel::ImageModel( QObject *parent ) :
    QAbstractTableModel( parent ), mPixelSize( 1 ), mDebugPixel( -1, -1 )
{
}

ImageModel::~ImageModel()
{

}

// helper function to check codel size
void
cc_check( int i, QRgb c, QRgb *last_c, int *last_p, int *min_w )
{
    if ( i == 0 ) {
        *last_c = c;
        *last_p = i;
    } else if ( *last_c != c ) {
        int w = i - *last_p;
        if ( w < *min_w ) {
            *min_w = w;
        }
        *last_c = c;
        *last_p = i;
    }
}

QImage ImageModel::autoScale(const QImage& _image, int codel_size)
{
    QImage image;
    if( _image.format() != QImage::Format_ARGB32_Premultiplied ) {
        image = _image.convertToFormat( QImage::Format_ARGB32_Premultiplied );
    } else {
        image = _image;
    }
    qDebug() << image.width() << image.height();
    if ( codel_size < 0 ) {
        /* Begin: modified part taken from npiet.c, cleanup_input () */
        // (C) 2010 Erik Schoenfelder <schoenfr@web.de>
        int i, j, last_p;
        QRgb last_cc;
        int min_w = image.width() + 1;
        int min_ww = image.width() + 1;
        int *o_cells;

        QRgb *cells = ( QRgb* ) image.bits();
        // scan image to guess codel size
        /* left to right: */
        for ( j = 0; j < image.height(); j++ ) {
            for ( i = 0; i < image.width(); i++ ) {
                cc_check( i, cells [j * image.width() + i], &last_cc, &last_p, &min_w );
            }
            cc_check( i, c_mark_index, &last_cc, &last_p, &min_w );
        }
        qDebug() << "his min_w" << min_w;
        /* top to bottom: */
        for ( i = 0; i < image.width(); i++ ) {
            for ( j = 0; j < image.height(); j++ ) {
                cc_check( j, cells [j * image.width() + i], &last_cc, &last_p, &min_w );
            }
            cc_check( j, c_mark_index, &last_cc, &last_p, &min_w );
        }
        codel_size = min_w;
        /* End: part taken from npiet.c, cleanup_input () */

        qDebug() << "Guessed codel size: " << codel_size;
    }
    // scale image so 1 codel == 1 pixel
    if ( codel_size > 1 ) {
        int width = image.width() / codel_size;
        int height = image.height() / codel_size;
        return image.scaled( width, height );
    } else {
        return image;
    }
}


void ImageModel::setImage( const QImage& image, int codel_size )
{
    mImage = autoScale(image, codel_size);
    qDebug() << mImage.width() << mImage.height();
    reset();
}

void ImageModel::newImage(int w, int h)
{
    QImage image( w, h, QImage::Format_ARGB32_Premultiplied );
    image.fill( QColor( Qt::white ).rgb() );
    setImage( image, 1 );
}

QImage ImageModel::image() const
{
    return mImage;
}

void ImageModel::insertImage(const QImage& _image, int x, int y, bool force)
{
    QImage newImage = autoScale(_image, -1);
    bool too_wide = ( x + newImage.size().width() ) >  imageSize().width();
    bool too_tall = ( y + newImage.size().height() ) >  imageSize().height();
    bool expand = false;
    if( !force && (too_tall || too_wide) ) {
        int but = QMessageBox::question( 0,
                                        tr( "Insert Image" ),
                                        tr( "This image will not fit inside the current source. Expand the current source?" ),
                                        QMessageBox::Yes | QMessageBox::No  | QMessageBox::Cancel, QMessageBox::Yes );
        if ( but == QMessageBox::Cancel ) {
            return;
        } else {
            expand = (but == QMessageBox::Yes); // we do want to expand if they click yes
        }
    }
    if( expand ) {
        int new_width = imageSize().width();
        int new_height = imageSize().height();
        if( too_wide ) {
            new_width = x + newImage.size().width();
        }
        if( too_tall ) {
            new_height = y + newImage.size().height();
        }
        scaleImage( QSize( new_width, new_height ) );
    }
    QPainter p( &mImage );
    p.drawImage( x, y, newImage );
    reset();
}

void ImageModel::setDebuggedPixel( int x, int y )
{
    emitNeighborsChanged( mDebugPixel.y(), mDebugPixel.x() );
    mDebugPixel.setY( y );
    mDebugPixel.setX( x );
    emitNeighborsChanged( mDebugPixel.y(), mDebugPixel.x() );
}

void ImageModel::emitNeighborsChanged( int row, int col )
{
    QModelIndex topLeft = index( row - 1,  col - 1 );
    QModelIndex bottomRight = index( row + 1,  col + 1 );
    emit dataChanged( topLeft, bottomRight );
}

int ImageModel::columnCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent )
    return mImage.width();
}

int ImageModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent )
    return mImage.height();
}


QVariant ImageModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    switch ( role ) {
    case Qt::DisplayRole: {
        QColor c;
        c.setRgb( mImage.pixel( index.column(), index.row() ) );
        return c;
    }
    case Qt::StatusTipRole:
        return statusString( index );
    case ImageModel::ContiguousBlocksRole:
        return contiguousBlocks( index.column(), index.row() );
    case ImageModel::IsCurrentDebugRole:
        return mDebugPixel.x() == index.column() && mDebugPixel.y() == index.row();
    default:
        return QVariant();
    }
}

QVariant ImageModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED( orientation )
    switch ( role ) {
    case Qt::SizeHintRole:
        return QSize( mPixelSize, mPixelSize );
    case Qt::DisplayRole:
        return QString::number( section );
    default:
        return QVariant();
    }
}

bool ImageModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if ( role != Qt::DisplayRole )
        return false;
    if ( !value.canConvert<QColor>() )
        return false;
    QColor c = value.value<QColor>();
    mImage.setPixel( index.column(), index.row(), c.rgb() );
    emit dataChanged( index, index );
    emit pixelChanged( index.column(), index.row(), c.rgb() );
    return true;
}


void ImageModel::slotPixelSizeChange( int size )
{
    mPixelSize = size;
}

QString ImageModel::statusString( QModelIndex index ) const
{
    QString coords;
    coords = QString( "X: %1 Y: %2" ).arg( index.column(), 3 ).arg( index.row(), 3 );

    quint64 connected = contiguousBlocks( index.column(), index.row() );

    QString character;
    if ( connected >= 32 && connected <= 126 )
        character = QString( "(char: '%1')" ).arg( ( char ) connected );
    return QString( "%1, contiguous: %2 %3" ).arg( coords ).arg( connected ).arg( character );
}


struct Frame {
    Frame( int _x, int _y ) : x( _x ), y( _y ) {}
    int x, y;
};

// emulate stack based iterative recursion to traverse the image data
// and find the number of contiguous blocks of the same color
// converted from an earlier 'actually' recursive algorithm.
// could probably use some optimization, however it seems to perform fine.
quint64 ImageModel::contiguousBlocks( int x, int y ) const
{
    if ( x < 0 || x >= mImage.width() || y < 0 || y >= mImage.height() )
        return 0;

    // array used to mark pixels as visited.
    // the image is mapped in row major fashion
    QScopedPointer<QStack<Frame*> > stack( new QStack<Frame*>() );
    QScopedPointer<QBitArray> markedArray( new QBitArray( mImage.width() * mImage.height() ) );
    quint64 result = 0;
    QRgb color = mImage.pixel( x, y );
    stack->push( new Frame( x, y  ) );
    while( !stack->isEmpty() ) {
        Frame* frame = stack->pop();
        if ( frame->x < 0 || frame->x >= mImage.width() || frame->y < 0 || frame->y >= mImage.height() ) {
            delete frame;
            continue;
        }
        if ( ( *markedArray )[frame->x*mImage.height()+frame->y] || mImage.pixel( frame->x, frame->y ) != color ) {
            delete frame;
            continue;
        }
        ( *markedArray )[frame->x*mImage.height()+frame->y] = 1;
        stack->push( new Frame( frame->x + 1, frame->y ) );
        stack->push( new Frame( frame->x - 1, frame->y ) );
        stack->push( new Frame( frame->x, frame->y + 1 ) );
        stack->push( new Frame( frame->x, frame->y - 1 ) );
        delete frame;
        result += 1;
    }
    return result;
}

void ImageModel::scaleImage( const QSize& size )
{
    QImage newImage(size, QImage::Format_ARGB32_Premultiplied);
    newImage.fill( QColor( Qt::white ).rgb() );
    QImage oldImage(mImage);
    mImage = newImage;
    insertImage(oldImage, 0,0, true);
}

QSize ImageModel::imageSize() const
{
    return mImage.size();
}

#include "ImageModel.moc"
