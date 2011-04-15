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

FlowCompass::FlowCompass( QWidget* parent ) :
        QWidget( parent ),
        mCCDirection( FlowCompass::Left ),
        mDPDirection( FlowCompass::Right ),
        mDPColor( 0xa4, 0x00, 0x00 ),
        mCCColor( 0x4e, 0x9a, 0x06 ),
        mArrowSize( 20 ),
        mPadding( 5 )
{
    setMinimumSize( 100, 100 );
}

FlowCompass::~FlowCompass()
{

}


QSize FlowCompass::sizeHint() const
{
    return QSize( 100, 100 );
}

void FlowCompass::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    paintDPArrow( painter );
    paintCCArrow( painter );

    QPen pen = QPen( Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    QFont font = painter.font();
    font.setBold( true );
    QFontMetrics fm( font );
    painter.setFont( font );

    pen.setColor( mDPColor );
    painter.setPen( pen );
    painter.drawText( mPadding, mPadding*2, "DP" );
    pen.setColor( mCCColor );
    painter.setPen( pen );
    painter.drawText( mPadding, mPadding*2 + fm.height(), "CC" );
}

void FlowCompass::paintCCArrow( QPainter &painter )
{
    // The CC arrow is relative to the DP.
    // So if the DP arrow is pointing right, and the CC is left,
    // then the absolute direction of the CC is up
    // This conversion is very ugly,
    // TODO find a more elegant solution
    Direction ccAbsoluteDirection;
    switch ( mDPDirection ) {
    case FlowCompass::Right:
        ccAbsoluteDirection = mCCDirection == FlowCompass::Right ? FlowCompass::Down : FlowCompass::Up;
        qDebug() << "DP Right" << "CC: " << ccAbsoluteDirection;
        break;
    case FlowCompass::Left:
        ccAbsoluteDirection = mCCDirection == FlowCompass::Right ? FlowCompass::Up : FlowCompass::Down;
        break;
    case FlowCompass::Up:
        ccAbsoluteDirection = mCCDirection == FlowCompass::Right ? FlowCompass::Right : FlowCompass::Left;
        break;
    case FlowCompass::Down:
        ccAbsoluteDirection = mCCDirection == FlowCompass::Right ? FlowCompass::Left : FlowCompass::Right;
        break;
    default:
        return;
    }
    QRect bounds( visibleRegion().boundingRect() );
    QSize boundsSz = bounds.size();
    int smallest_side = qMin( boundsSz.width(), boundsSz.height() );
    boundsSz.scale( smallest_side - mArrowSize - mPadding, smallest_side - mArrowSize - mPadding, Qt::IgnoreAspectRatio);
    bounds.setSize( boundsSz );
    bounds.moveCenter( visibleRegion().boundingRect().center() );
    QPoint center( bounds.center() );
    QPoint end;
    switch ( ccAbsoluteDirection ) {
    case FlowCompass::Right:
        end.setX( bounds.right() );
        end.setY( center.y() );
        break;
    case FlowCompass::Left:
        end.setX( bounds.left() );
        end.setY( center.y() );
        break;
    case FlowCompass::Up:
        end.setX( center.x() );
        end.setY( bounds.top() );
        break;
    case FlowCompass::Down:
        end.setX( center.x() );
        end.setY( bounds.bottom() );
        break;
    default:
        return;
    }

    QLine dpLine( center, end );
    QVector<QPoint> arrowHead = calculateArrowHead( dpLine, bounds, mArrowSize, ccAbsoluteDirection );

    painter.save();
    QPen pen = QPen( Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    painter.setPen( pen );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setBrush( mCCColor );
    painter.drawLine( dpLine );
    painter.drawPolygon( arrowHead );
    pen.setColor( Qt::black );
    painter.restore();
}

void FlowCompass::paintDPArrow( QPainter &painter )
{
    QRect bounds( visibleRegion().boundingRect() );
    QSize boundsSz = bounds.size();
    int smallest_side = qMin( boundsSz.width(), boundsSz.height() );
    boundsSz.scale( smallest_side - mArrowSize - mPadding, smallest_side - mArrowSize - mPadding, Qt::IgnoreAspectRatio);
    bounds.setSize( boundsSz );
    bounds.moveCenter( visibleRegion().boundingRect().center() );
    QPoint center( bounds.center() );
    QPoint end;
    switch ( mDPDirection ) {
    case FlowCompass::Right:
        end.setX( bounds.right() );
        end.setY( center.y() );
        break;
    case FlowCompass::Left:
        end.setX( bounds.left() );
        end.setY( center.y() );
        break;
    case FlowCompass::Up:
        end.setX( center.x() );
        end.setY( bounds.top() );
        break;
    case FlowCompass::Down:
        end.setX( center.x() );
        end.setY( bounds.bottom() );
        break;
    default:
        return;
    }

    QLine dpLine( center, end );
    QVector<QPoint> arrowHead = calculateArrowHead( dpLine, bounds, mArrowSize, mDPDirection );

    painter.save();
    QPen pen = QPen( Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    painter.setPen( pen );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setBrush( mDPColor );
    painter.drawLine( dpLine );
    painter.drawPolygon( arrowHead );
    painter.restore();
}

QVector<QPoint>  FlowCompass::calculateArrowHead( QLine line, QRect bounds, qreal arrowSize, Direction direction )
{
    double angle = ::acos( (double)( line.dx() / bounds.width() ) );
    if ( line.p2().x() < line.p1().x() || line.p2().y() < line.p1().y() )
        angle = ( Pi * 2 ) - angle;

    QPoint arrowP1, arrowP2;
    QVector<QPoint> arrowHead;

    if ( direction == FlowCompass::Right || direction == FlowCompass::Left )  {
        arrowP1 = line.p2() + QPoint( cos( angle + Pi / 3 ) * arrowSize,
                                      sin( angle + Pi / 3 ) * arrowSize );
        arrowP2 = line.p2() + QPoint( cos( angle + Pi - Pi / 3 ) * arrowSize,
                                      sin( angle + Pi - Pi / 3 ) * arrowSize );
    } else {
        arrowP1 = line.p2() + QPoint( sin( angle + Pi / 3 ) * arrowSize,
                                      cos( angle + Pi / 3 ) * arrowSize );
        arrowP2 = line.p2() + QPoint( sin( angle + Pi - Pi / 3 ) * arrowSize,
                                      cos( angle + Pi - Pi / 3 ) * arrowSize );
    }
    arrowHead << line.p2() << arrowP1 << arrowP2;
    return arrowHead;
}


void FlowCompass::setCCDirection( const FlowCompass::Direction& direction )
{
    mCCDirection = direction;
    update();
}

void FlowCompass::setDPDirection( const FlowCompass::Direction& direction )
{
    mDPDirection = direction;
    update();
}


#include "FlowCompass.moc"

