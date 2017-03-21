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

#include "UndoCommands.h"

#include "ImageModel.h"

#include <QDebug>

EditPixelCommand::EditPixelCommand(int x, int y, QColor old_color, QColor new_color, ImageModel * model, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mX(x)
    , mY(y)
    , mBefore(old_color)
    , mAfter(new_color)
    , mModel(model)
{

}

void EditPixelCommand::redo()
{
    mModel->setData( mModel->index(mY, mX), mAfter, Qt::DisplayRole );
}

void EditPixelCommand::undo()
{
    qDebug() << "me undo: " << mBefore << mAfter;
    mModel->setData( mModel->index(mY, mX), mBefore, Qt::DisplayRole );
}

bool EditPixelCommand::mergeWith(const QUndoCommand* command)
{
    const EditPixelCommand *editPixelCommand = static_cast<const EditPixelCommand *>(command);
    if( editPixelCommand->mX == mX && editPixelCommand->mY == mY )
        return mAfter == editPixelCommand->mAfter;
    return false;
}

InsertImageCommand::InsertImageCommand(int x, int y, QImage before, QImage imageToInsert, QSize after, ImageModel* model, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mX(x)
    , mY(y)
    , mBefore(before)
    , mAfter(after)
    , mImageToInsert(imageToInsert)
    , mModel(model)
{
}

void InsertImageCommand::redo()
{
    mModel->scaleImage(mAfter);
    mModel->insertImage(mImageToInsert, mX, mY);
}

void InsertImageCommand::undo()
{
    mModel->setImage(mBefore, 1);
}

ScaleImageCommand::ScaleImageCommand(QImage before, QSize after, ImageModel* model, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mBefore(before)
    , mAfter(after)
    , mModel(model)
{

}

void ScaleImageCommand::redo()
{
    mModel->scaleImage(mAfter);
}

void ScaleImageCommand::undo()
{
    mModel->setImage(mBefore, 1);
}


