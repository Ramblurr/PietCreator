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

#include "DebugWidget.h"

extern "C"
{
#include "npiet/npiet_utils.h"
}

#include "ImageModel.h"
#include "CommandImpl.h"

#include <QDebug>

DebugWidget::DebugWidget( ImageModel* model, QWidget* parent, Qt::WindowFlags f ): QWidget( parent, f ), mImageModel( model )
{
    setupUi( this );
}

DebugWidget::~DebugWidget()
{

}

void DebugWidget::slotActionChanged( trace_action* action )
{
    mActionLabel->setText( command( action->light_change, action->hue_change ).name );
}

void DebugWidget::slotStepped( trace_step* step )
{
    mCoordinate->setText( QString("%1,%2").arg(step->p_xpos).arg( step->p_ypos) );
    mImageModel->setDebuggedPixel( step->p_xpos, step->p_ypos );

    quint64 connected = mImageModel->data( mImageModel->index( step->p_ypos, step->p_xpos ), ImageModel::ContiguousBlocksRole ).toInt();
    QString character;
    if ( connected >= 32 && connected <= 126 )
        character = QString( "(char: '%1')" ).arg(( char ) connected );
    QString value = QString( "%1 %2" ).arg( connected ).arg( character );
    mValueLabel->setText( value );
}

Command DebugWidget::command( int light_change, int hue_change )
{
    switch (hue_change) {
    case 0:
        /*  None                  push        pop     */
        if (light_change == 0) {
            /*
            * noop - nothing to do (should not happen)
            */
            return PietCommand::Noop();
        } else if (light_change == 1) {
            /*
            push: Pushes the value of the colour block just exited on to the
            stack. Note that values of colour blocks are not automatically
            pushed on to the stack - this push operation must be explicitly
            carried out.
            */
            return PietCommand::Push();
        } else if (light_change == 2) {
            /*
            pop: Pops the top value off the stack and discards it.
            */
            return PietCommand::Pop();
        }
        break;
    case 1:
        /*     1 Step       add    subtract   multiply */
        if (light_change == 0) {
            /*
            add: Pops the top two values off the stack, adds them, and pushes
            the result back on the stack.
            */
            return PietCommand::Add();
        } else if (light_change == 1) {
            /*
            subtract: Pops the top two values off the stack, subtracts the top
            value from the second top value, and pushes the result back on the
            stack.
            */
            return PietCommand::Subtract();
        } else if (light_change == 2) {
            /*
            multiply: Pops the top two values off the stack, multiplies them,
            and pushes the result back on the stack.
            */
            return PietCommand::Multiply();
        }
        break;
    case 2:
        /*    2 Steps    divide         mod        not */
        if (light_change == 0) {
            /*
            divide: Pops the top two values off the stack, calculates the
            integer division of the second top value by the top value, and
            pushes the result back on the stack.
            */
            return PietCommand::Divide();
        } else if (light_change == 1) {
            /*
            mod: Pops the top two values off the stack, calculates the second
            top value modulo the top value, and pushes the result back on the
            stack.
            */
            return PietCommand::Mod();
        } else if (light_change == 2) {
            /*
            not: Replaces the top value of the stack with 0 if it is non-zero,
            and 1 if it is zero.
            */
            return PietCommand::Not();
        }
        break;
    case 3:
        /*    3 Steps   greater     pointer     switch */
        if (light_change == 0) {
            /*
            greater: Pops the top two values off the stack, and pushes 1 on to
            the stack if the second top value is greater than the top value,
            and pushes 0 if it is not greater.
            */
            return PietCommand::Greater();
        } else if (light_change == 1) {
            /*
            pointer: Pops the top value off the stack and rotates the DP
            clockwise that many steps (anticlockwise if negative).
            */
            return PietCommand::Pointer();
        } else if (light_change == 2) {
            /*
            switch: Pops the top value off the stack and toggles the CC that
                many times.
                */
            return PietCommand::Switch();
        }
        break;
    case 4:
        /*    4 Steps  duplicate  roll  in(number) */
        if (light_change == 0) {
            /*
            duplicate: Pushes a copy of the top value on the stack on to the
            stack.
            */
            return PietCommand::Duplicate();
        } else if (light_change == 1) {
            /*
            roll: Pops the top two values off the stack and "rolls" the
            remaining stack entries to a depth equal to the second value
            popped, by a number of rolls equal to the first value popped. A
            single roll to depth n is defined as burying the top value on the
            stack n deep and bringing all values above it up by 1 place. A
            negative number of rolls rolls in the opposite direction. A
            negative depth is an error and the command is ignored.
            */
            return PietCommand::Roll();
        } else if (light_change == 2) {
            /*
            in: Reads a value from STDIN as either a number or character,
            depending on the particular incarnation of this command and pushes
            it on to the stack.
            */
            return PietCommand::In();
        }
        break;
    case 5:
        /*    5 Steps  in(char) out(number)  out(char) */
        if (light_change == 0) {
            /*
            in: Reads a value from STDIN as either a number or character,
            depending on the particular incarnation of this command and pushes
            it on to the stack.
            */
            return PietCommand::In();
        } else if (light_change == 1) {
            /*
            out: Pops the top value off the stack and prints it to STDOUT as
            either a number or character, depending on the particular
            incarnation of this command.
            */
            return PietCommand::Out();
        } else if (light_change == 2) {
            /*
            out: Pops the top value off the stack and prints it to STDOUT as
            either a number or character, depending on the particular
            incarnation of this command.
            */
            return PietCommand::Out();
        }
        break;
    }
}


#include "DebugWidget.moc"
