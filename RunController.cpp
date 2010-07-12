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

#include "RunController.h"

extern "C"
{
  #include "npiet/npiet.h"
}

RunController::RunController(ImageModel* model, QObject* parent): QObject( parent ), mImageModel( model ), mPrepared( false )
{

}


void RunController::run()
{
    if( !mImageModel )
        return;
    if( !mPrepared && !prepare() )
        return;
    piet_run();
}

bool RunController::prepare()
{
    QImage image = mImageModel->image();
//     image = image.convertToFormat( QImage::Format_RGB32 );
    QRgb* cells = (QRgb*) image.bits();
    set_image( image.width(), image.height());
    for( int i = 0; i < image.height(); ++i ) {
      QRgb* lineptr = (QRgb*) image.scanLine( i );
      for( int j = 0; j < image.width(); ++j ) {
        int r = qRed( lineptr[j] );
        int g = qGreen( lineptr[j] );
        int b = qBlue( lineptr[j] );
        int col = ((r * 256 + g) * 256) + b;
        int col_idx = get_color_idx (col);
        if (col_idx < 0) {
          /* set to black or white: */
          col_idx = (/*unknown_color*/1 == 0 ? c_black : c_white);
        }
        set_cell (j, i, col_idx);
      }
    }
    mPrepared = true;
    return mPrepared;
}

#include "RunController.moc"
