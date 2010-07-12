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
    QImage image( "nhello.ppm" );

    qDebug() << image.bits()  << image.width() << image.height();
    unsigned int * cells = (unsigned int*) image.bits();
    set_image( cells, image.width(), image.height() );
//     read_ppm("nhello.ppm");
    qDebug() << "result:" << piet_run();
}

QTEST_MAIN( NPietTest )

#include "NPietTest.moc"