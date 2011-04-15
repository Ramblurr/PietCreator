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

#include <QUndoCommand>
#include <QImage>
#include <QColor>

class ImageModel;

class EditPixelCommand : public QUndoCommand
{
public:
    enum { Id = 1 };
    EditPixelCommand(int x, int y, QColor old_color, QColor new_color, ImageModel* model, QUndoCommand* parent = 0 );
    void undo();
    void redo();
    bool mergeWith(const QUndoCommand *command);
    int id() const { return Id; }
private:
    int mX, mY;
    QColor mBefore,mAfter;
    ImageModel *mModel;
    
};

class InsertImageCommand : public QUndoCommand
{
public:
    InsertImageCommand(int x, int y, QImage before, QImage imageToInsert, ImageModel* model, QUndoCommand* parent = 0 );
    void undo();
    void redo();
private:
    int mX, mY;
    QImage mBefore,mImageToInsert;
    ImageModel *mModel;
};