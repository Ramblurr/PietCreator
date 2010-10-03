/*
 *  Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
 * 
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or (at your
 *  option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *  License for more details.
 * 
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301, USA.
 */

#include "FlowCompass.h"

#include <QPainter>
#include <QDebug>
#include <QPolygon>

#include <math.h>

const qreal Pi = 3.14;

FlowCompass::FlowCompass(QWidget* parent): QWidget(parent), mCCDirection( FlowCompass::Left ), mDPDirection( FlowCompass::Right )
{
    setMinimumSize(200,200);
}

FlowCompass::~FlowCompass()
{

}


QSize FlowCompass::sizeHint() const
{
    return QSize( 200, 150 );
}

void FlowCompass::paintEvent(QPaintEvent* )
{
    QPainter painter( this );
    QPen myPen = QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    qreal arrowSize = 20;
    painter.setPen(myPen);
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setBrush(Qt::black);

    QSize bounds( visibleRegion().boundingRect().width() - arrowSize,
                  visibleRegion().boundingRect().height() - arrowSize );

    QPoint center( bounds.width() / 2, bounds.height() / 2);
    QPoint end;

    switch( mDPDirection ) {
        case FlowCompass::Right:
            end.setX( bounds.width() );
            end.setY( center.y() );
            break;
        case FlowCompass::Left:
            end.setX( 0 );
            end.setY( center.y() );
            break;
        case FlowCompass::Up:
            end.setX( center.x() );
            end.setY(0);
            break;
        case FlowCompass::Down:
            end.setX( center.x() );
            end.setY( bounds.width() );
            break;
        default:
            return;
    }

    QLine dpLine(center, end);
    double angle = ::acos(dpLine.dx() / bounds.width() );
    if( end.x() < center.x() || end.y() < center.y() )
        angle = (Pi * 2) - angle;

    QPoint arrowP1, arrowP2;
    QVector<QPoint> arrowHead;

    if( mDPDirection == FlowCompass::Right || mDPDirection == FlowCompass::Left )  {
        arrowP1 = dpLine.p2() + QPoint(cos(angle + Pi / 3) * arrowSize,
                                       sin(angle + Pi / 3) * arrowSize);
        arrowP2 = dpLine.p2() + QPoint(cos(angle + Pi - Pi / 3) * arrowSize,
                                       sin(angle + Pi - Pi / 3) * arrowSize);
    } else {
        arrowP1 = dpLine.p2() + QPoint(sin(angle + Pi / 3) * arrowSize,
                                       cos(angle + Pi / 3) * arrowSize);
        arrowP2 = dpLine.p2() + QPoint(sin(angle + Pi - Pi / 3) * arrowSize,
                                       cos(angle + Pi - Pi / 3) * arrowSize);
    }
    arrowHead.clear();
    arrowHead << dpLine.p2() << arrowP1 << arrowP2;
    painter.drawLine(dpLine);
    painter.drawPolygon(arrowHead);
}

void FlowCompass::setCCDirection(const FlowCompass::Direction& direction)
{
    mCCDirection = direction;
    update();
}

void FlowCompass::setDPDirection(const FlowCompass::Direction& direction)
{
    mDPDirection = direction;
    update();
}


#include "FlowCompass.moc"

