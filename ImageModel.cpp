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

#include <QtGui>
#include <QDebug>

ImageModel::ImageModel( QObject *parent ) :
        QAbstractTableModel( parent ), mPixelSize( 1 )
{
}

ImageModel::~ImageModel()
{

}

void ImageModel::setImage( const QImage& image )
{
    mImage = image;
    reset();
}

QImage ImageModel::image() const
{
    return mImage;
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
    if ( !index.isValid() || role != Qt::DisplayRole )
        return QVariant();
    QColor c;
    c.setRgb( mImage.pixel( index.column(), index.row() ) );
    return c;
}

QVariant ImageModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED( orientation )
    switch( role ) {
      case Qt::SizeHintRole:
        return QSize(mPixelSize, mPixelSize);
      case Qt::DisplayRole:
        return QString::number( section );
      default: 
        return QVariant();
    }
}

void ImageModel::slotPixelSizeChange(int size)
{
  mPixelSize = size;
}


#include "ImageModel.moc"
