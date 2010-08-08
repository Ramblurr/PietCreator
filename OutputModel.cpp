/*
    Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
    Copyright 2007 Andreas Pakulat <apaku@gmx.de>

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

#include "OutputModel.h"

#include <QStringList>
#include <QTextStream>

OutputModel::OutputModel( QObject* parent )
    : QStandardItemModel( parent )
{

}

void OutputModel::appendString( const QString& text )
{
    if( this->item( rowCount() - 1 ) == 0 ) {
        appendLine( text );
        return;
    }
    QByteArray ba = text.toAscii();
    QTextStream s( &ba );
    QString buffer;
    while( !s.atEnd() ) {
        QChar c;
        s >> c;
        if( c.category() == QChar::Separator_Line ) {
            appendLine( buffer );
            buffer.clear();
        } else {
            buffer.append( c );
        }
    }
    if( !buffer.isEmpty() ) {
        QStandardItem* item = this->item( rowCount() - 1 );
        item->setText( item->text().append( buffer ) );
    }
}

void OutputModel::appendLine( const QString& line )
{
    QStandardItem* item = new QStandardItem( line );
    appendRow( item );
}

void OutputModel::appendLines( const QStringList& lines )
{
    Q_FOREACH( const QString & s, lines ) {
        appendLine( s );
    }
}

#include "OutputModel.moc"
