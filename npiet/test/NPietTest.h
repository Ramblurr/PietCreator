#ifndef NPIETTEST_H
#define NPIETTEST_H

#include <QObject>
class NPietTest : public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();
  void simpleTest();
};

#endif

