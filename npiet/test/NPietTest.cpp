#include "NPietTest.h"

extern "C"
{
#include "../npiet.h"
}

#include <QtTest/QTest>
#include <QImage>
#include <QDebug>

void NPietTest::initTestCase()
{
}

void NPietTest::simpleTest()
{
    QImage image( "nhello.png" );
    image = image.convertToFormat( QImage::Format_RGB32 );
    qDebug() << image.bits()  << image.width() << image.height();
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
    qDebug() << "result:" << piet_run();
}

QTEST_MAIN( NPietTest )

#include "NPietTest.moc"