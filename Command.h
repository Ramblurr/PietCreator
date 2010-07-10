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

#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QColor>
#include <QMetaType>

class QColor;

struct Command
{
    Command(){}
    Command( const QString &_name, const QString &_desc ) : name( _name ), desc( _desc ) {}
    Command( const QString &_name, const QString &_desc, const QColor &_color, int _index ) : name( _name ), desc( _desc ), color( _color ), index( _index ) {}

    Command( const Command &other ) {
      name = other.name;
      desc = other.desc;
      color = other.color;
      index = other.index;
    }

    Command &operator=( const Command &other ) {
        if( &other != this ) {
          name = other.name;
          desc = other.desc;
          color = other.color;
          index = other.index;
        }
        return *this;
    }

    QString name;
    QString desc;
    QColor  color;
    int     index;
};

Q_DECLARE_METATYPE(Command)

#endif // COMMAND_H
