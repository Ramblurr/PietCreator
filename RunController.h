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

#ifndef RUNCONTROLLER_H
#define RUNCONTROLLER_H

#include "ImageModel.h"

#include <QObject>
#include <QTextStream>

class QSocketNotifier;

class RunController : public QObject
{
    Q_OBJECT
public:
    RunController( ImageModel * model, QObject* parent );

    /**
     Take the program image, pass it to the npiet interpreter
     and execute it.
     */
    void run();

    void initialize();

signals:
    void newOutput( const QString &);

private slots:
    void stdoutReadyRead();
private:
    bool prepare();
    void finish();
    QTextStream* mStdOut;
    QSocketNotifier* mNotifier;
    ImageModel* mImageModel;
    bool mPrepared;
    int mPipeFd[2]; /**< [0] is read end, [1] is write end */
    int mOrigFd;
    int mOrigFdCopy;
};

#endif // RUNCONTROLLER_H
