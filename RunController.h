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

#include <QObject>
#include <QTextStream>
#include <QImage>

class QSocketNotifier;
class NPietObserver;

struct trace_step;
struct trace_action;

class RunController : public QObject
{
    Q_OBJECT
public:
    RunController();

signals:
    void newOutput( const QString &);
    void stepped( trace_step* );
    void actionChanged( trace_action* );

public slots:
    /**
     Call this right before calling execute
     */
    bool initialize( const QImage &source );
    /**
    Take the program image, pass it to the npiet interpreter
    and execute it.
    */
    void execute();

    bool initializeAndExecute( const QImage &source );
private slots:
    void stdoutReadyRead();
private:
    void captureStdout();
    bool prepare();
    void finish();

    QTextStream* mStdOut;
    QSocketNotifier* mNotifier;
    NPietObserver* mObserver;
    bool mPrepared;
    int mPipeFd[2]; /**< [0] is read end, [1] is write end */
    int mOrigFd;
    int mOrigFdCopy;
    QImage mSource;
};

#endif // RUNCONTROLLER_H
