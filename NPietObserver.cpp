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

#include "NPietObserver.h"
#include <QDebug>
extern "C"
{
    #include "npiet/npiet_utils.h"
}

NPietObserver::NPietObserver( RunController* controller ): QObject( controller ), mRunController( controller )
{
    register_step_callback( call_step, this );
    register_action_callback( call_action, this );
    register_readchar_callback( call_readchar, this );
    register_readint_callback( call_readint, this );
}

void NPietObserver::action( trace_action* act )
{
    emit actionChanged( act );
}

void NPietObserver::step( trace_step* ste )
{
    emit stepped( ste );
}

char NPietObserver::get_char()
{
    return mRunController->getChar();
}

int NPietObserver::get_int()
{
    qDebug() << "HAI";
    qDebug() << thread();
    return mRunController->getInt();
}

void NPietObserver::call_action( void* object, trace_action* act )
{
    NPietObserver* me = (NPietObserver*) object;
    me->action( act );
}

void NPietObserver::call_step( void* object, trace_step* ste )
{
    NPietObserver* me = (NPietObserver*) object;
    me->step( ste );
}

char NPietObserver::call_readchar( void* object )
{
    NPietObserver* me = (NPietObserver*) object;
    return me->get_char();
}

int NPietObserver::call_readint( void* object )
{
    NPietObserver* me = (NPietObserver*) object;
    return me->get_int();
}


#include "NPietObserver.moc"
