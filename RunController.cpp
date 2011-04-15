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

#include "NPietObserver.h"

#include <QSocketNotifier>
#include <QDebug>
#include <QTemporaryFile>
#include <QThread>

#include <stdio.h>
#include <iostream>
#include <fcntl.h>

#ifdef Q_WS_WIN
#include <io.h>
#else
#include <unistd.h>
#endif


extern "C"
{
#include "npiet/npiet.h"
#include "npiet/npiet_utils.h"
}

RunController::RunController(): QObject( 0 ), mPrepared( false ), mStdOut( 0 ), mNotifier( 0 ), mObserver( 0 ), mAbort( false ), mExecuting( false ), mDebugging( false )
{
}

RunController::~RunController()
{
    qDebug() << "~RunController";
    mMutex.lock();
    mAbort = true;
    qDebug() << "waking";
    mWaitCond.wakeOne();
    mMutex.unlock();
    thread()->wait();
}

void RunController::slotThreadStarted()
{
    mObserver = new NPietObserver( this );
    connect( mObserver, SIGNAL( stepped( trace_step* ) ), this,SLOT( slotStepped(trace_step* ) ) );
    connect( mObserver, SIGNAL( actionChanged( trace_action* ) ), this, SLOT( slotAction( trace_action* ) ) );
    connect( &mTimer, SIGNAL( timeout() ), this, SLOT( tick() ) );
}


bool RunController::initialize( const QImage &source )
{
    mSource = source;
    captureStdout();
    if( prepare() ) {
        piet_init();
        return true;
    }
    return false;
}

void RunController::execute()
{
    mExecuting = true;
    if ( !mPrepared )
        return;
    mTimer.start( 0 );
}

void RunController::tick()
{
    bool abort;
    QMutexLocker locker( &mMutex );
//     if( mAbort ) {
//         abort = true;
//     }
    if( !mAbort && piet_step() < 0 )
        mAbort = true;
    if( mAbort ) {
        mTimer.stop();
        finish();
        mAbort = false;
        emit stopped();
        return;
    }
}

void RunController::step()
{
    if ( !mPrepared )
        return;
    int res = piet_step();
}

void RunController::abort()
{
    qDebug() << "abort!";
    QMutexLocker locker( &mMutex );
    stop();
}

void RunController::stop()
{
    qDebug() << "stop!";
    if( mExecuting ) {
        mAbort = true;
        mPrepared = false;
        mWaitCond.wakeOne();
    } else if( mDebugging ) {
        // TODO reset npiets internal state?
        finish();
        emit stopped();
    }
}

bool RunController::runSource( const QImage& source )
{
    mMutex.lock();
    stop();
    mExecuting = true;
    mMutex.unlock();
    if ( initialize( source ) ) {
        execute();
        return true;
    }
    abort();
    mMutex.lock();
    finish();
    mMutex.unlock();
    return false;
}

void RunController::debugSource( const QImage& source )
{
    mMutex.lock();
    stop();
    mDebugging = true;
    mMutex.unlock();
    if( !initialize( source ) )
        abort();
    emit debugStarted();
}

void RunController::pixelChanged(int x, int y, QRgb color)
{
    if(mPrepared) {
        mSource.setPixel( x, y, color );
        int col = ( ( qRed(color) * 256 + qGreen(color) ) * 256 ) + qBlue(color);
        int col_idx = get_color_idx( col );
        if ( col_idx < 0 ) {
            /* set to black or white: */
            col_idx = ( /*unknown_color*/1 == 0 ? c_black : c_white );
        }
        set_cell( x, y, col_idx );
    }        
}

void RunController::finish()
{
    mTimer.stop();
    mExecuting = false;
    mDebugging = false;
    mPrepared = false;

    if ( mNotifier == 0 ) return;

    std::cout.flush(); // flush standard output cout file descriptor
    ::fflush( NULL );  // flush all file buffers
#ifndef Q_WS_WIN
    ::fsync( 1 );      // syncronize standard output buffers -- may be unnessessery
#endif
    stdoutReadyRead();        // process whatever data is left there

    // Restore original state
    delete mNotifier;
    delete mStdOut;
    mNotifier = 0;
    mStdOut = 0;

    ::dup2( mOrigFdCopy, mOrigFd ); // restore the output descriptor
    ::close( mOrigFdCopy ); // close the copy as it's redundant now
    ::close( mPipeFd[0] );  // close the reading end of the pipe
}

bool RunController::prepare()
{
//     image = mSource.convertToFormat( QImage::Format_RGB32 );
    QRgb* cells = ( QRgb* ) mSource.bits();
    set_image( mSource.width(), mSource.height() );
    for ( int i = 0; i < mSource.height(); ++i ) {
        QRgb* lineptr = ( QRgb* ) mSource.scanLine( i );
        for ( int j = 0; j < mSource.width(); ++j ) {
            int r = qRed( lineptr[j] );
            int g = qGreen( lineptr[j] );
            int b = qBlue( lineptr[j] );
            int col = ( ( r * 256 + g ) * 256 ) + b;
            int col_idx = get_color_idx( col );
            if ( col_idx < 0 ) {
                /* set to black or white: */
                col_idx = ( /*unknown_color*/1 == 0 ? c_black : c_white );
            }
            set_cell( j, i, col_idx );
        }
    }
    mPrepared = true;
    return mPrepared;
}

void RunController::stdoutReadyRead()
{
    emit newOutput( mStdOut->readAll() );
}

void RunController::slotAction( trace_action* act )
{
    if( mDebugging )
        emit actionChanged( act );
}

void RunController::slotStepped( trace_step* step )
{
if( mDebugging )
        emit stepped( step );
}

char RunController::getChar()
{
//     QMutexLocker locker( &mMutex );
    emit waitingForChar();
    mWaitCond.wait( &mMutex );
    return mChar;
}

int RunController::getInt()
{
//     qDebug() << "getInt()" << "getting lock";
//     QMutexLocker locker( &mMutex );
//     qDebug() << "getInt()" << "got lock";
    emit waitingForInt();
//     qDebug() << "getInt()" << "waiting";
    bool timer_running = mTimer.isActive();
    if( timer_running )
        mTimer.stop();
    mWaitCond.wait( &mMutex );
    qDebug() << "getInt()" << "woke up!" << mInt;
    if( timer_running )
        mTimer.start();
    return mInt;
}

void RunController::putChar( const QChar & c )
{
    qDebug() << "putChar";
    QMutexLocker locker( &mMutex );
    mChar = c.toAscii();
    mWaitCond.wakeOne();
}

void RunController::putInt( int i )
{
    QMutexLocker locker( &mMutex );
    qDebug() << "putInt" << i;
    mInt = i;
    mWaitCond.wakeOne();
}

//
// Downloaded from: http://www.ibib.waw.pl/~winnie
//
// License: Public domain
//
void RunController::captureStdout()
{
    /**
    Capture stdout in a cross platform way (I hope).
    1. Initialize pipe.
    2. Dup STDOUT so we can restore it later.
    3. Make the stdout descriptor a copy of pipe's write end.
    4. Close write end.
    5. Set flags to ensure non blocking on pipe's read end
    6. Associate pipe read end with a FILE*
    7. Pass the FILE* to qtextstream
    8. Create a QSocketNotifier on the pipe's read end to monitor for new data
    */
#ifdef Q_WS_WIN
    int rc = ::_pipe( mPipeFd, 1024, _O_TEXT );
#else
    int rc = ::pipe( mPipeFd );
#endif
    Q_ASSERT( rc >= 0 );

#ifdef Q_WS_WIN
	mOrigFd = _fileno( stdout );
#else
    mOrigFd = STDOUT_FILENO;
#endif

    mOrigFdCopy = ::dup( mOrigFd );
    Q_ASSERT( mOrigFdCopy >= 0 );

    rc = ::dup2( mPipeFd[1], mOrigFd );
    Q_ASSERT( rc >= 0 );
    ::close( mPipeFd[1] );

#ifndef Q_WS_WIN
    rc = ::fcntl( mPipeFd[0], F_GETFL );
    Q_ASSERT( rc != -1 );
    rc = ::fcntl( mPipeFd[0], F_SETFL, rc | O_NONBLOCK );
    Q_ASSERT( rc != -1 );
#endif
    FILE * f = fdopen( mPipeFd[0], "r" );
    Q_ASSERT( f != 0 );

    if ( mStdOut != 0 )
        delete mStdOut;
    if ( mNotifier != 0 )
        delete mNotifier;
    mStdOut = new QTextStream( f );
    mNotifier = new QSocketNotifier( mPipeFd[0], QSocketNotifier::Read );
    QObject::connect( mNotifier, SIGNAL( activated( int ) ), SLOT( stdoutReadyRead() ) );
}

#include "RunController.moc"
