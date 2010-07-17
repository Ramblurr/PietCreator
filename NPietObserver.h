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

#ifndef NPIETOBSERVER_H
#define NPIETOBSERVER_H

#include <QObject>

struct trace_step;
struct trace_action;

class NPietObserver : public QObject
{
    Q_OBJECT
public:
    NPietObserver( QObject* parent = 0 );

    void step( struct trace_step * );
    void action( struct trace_action * );

    static void call_step( void* object, struct trace_step * );
    static void call_action( void* object, struct trace_action * );

signals:
    void stepped( trace_step* );
    void actionChanged( trace_action* );
};

#endif // NPIETOBSERVER_H
