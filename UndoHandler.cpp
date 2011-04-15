#include "UndoHandler.h"

#include "ImageModel.h"

#include "UndoCommands.h"

#include <QUndoStack>

UndoHandler::UndoHandler( QUndoStack* undostack, ImageModel* model ) : mUndoStack( undostack ), mModel( model )
{
}

void UndoHandler::createEditPixel(int x, int y, QColor new_color)
{
    
    QColor old_color = mModel->data( mModel->index(y, x), Qt::DisplayRole ).value<QColor>();
    EditPixelCommand *cmd = new EditPixelCommand(x, y, old_color, new_color, mModel );
    mUndoStack->push(cmd);
}

void UndoHandler::insertImage(int x, int y, QImage imageToInsert)
{
    QImage before = mModel->image();
    InsertImageCommand *cmd = new InsertImageCommand(x,y,before, imageToInsert, mModel);
    mUndoStack->push(cmd);
}


