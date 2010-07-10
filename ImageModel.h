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
#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractTableModel>
#include <QImage>
class QBitArray;
class ImageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ImageModel( QObject *parent = 0 );
    virtual ~ImageModel();

    /** Sets the image to expose via the model
        If codel size is not specified, then the model
    */
    void setImage( const QImage &image, int codel_size = -1 );
    QImage image() const;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

public slots:
    /*! This is a very bad hack.
        headerData() needs to know the current pixel size (IN THE VIEW!) in order
        to return a correct SizeHint, otherwise things get screwy.
        Note: this pixel information is only used by headerData to return a SizeHint
        It does not affect the backend image data at all
        */
    void slotPixelSizeChange( int size );
private:
    QString statusString( QModelIndex index ) const;
    int contiguousBlocks( int x, int y ) const;
    int contiguousBlocks( int x, int y, QRgb color, QBitArray &markedArray  ) const;
    QImage mImage;
    int mPixelSize;
};

#endif // IMAGEMODEL_H
