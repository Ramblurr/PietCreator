#include "UndoHandler.h"

#include "ImageModel.h"

#include "UndoCommands.h"

#include <QUndoStack>
#include <QDebug>

UndoHandler::UndoHandler( QUndoStack* undostack, ImageModel* model ) : mUndoStack( undostack ), mModel( model )
{
}

void UndoHandler::createEditPixel(int x, int y, QColor new_color, bool dragging)
{
    qDebug() << "createEditPixel" << dragging;
    EditPixelCommand* parent = 0;
    if (dragging) {
        const QUndoCommand* cmd = mUndoStack->command( mUndoStack->index() );
        const EditPixelCommand* const_parent = dynamic_cast<const EditPixelCommand*>(cmd);
        qDebug() << const_parent;
        if( const_parent )
            parent = const_cast<EditPixelCommand*>(const_parent);
    }
    QColor old_color = mModel->data( mModel->index(y, x), Qt::DisplayRole ).value<QColor>();
    EditPixelCommand *cmd = new EditPixelCommand(x, y, old_color, new_color, mModel, parent );
    mUndoStack->push(cmd);
}

void UndoHandler::insertImage(int x, int y, QImage imageToInsert)
{
    QImage before = mModel->image();
    InsertImageCommand *cmd = new InsertImageCommand(x,y,before, imageToInsert, mModel);
    mUndoStack->push(cmd);
}

void UndoHandler::scaleImage(QSize newSize)
{
    QSize before = mModel->imageSize();
    ScaleImageCommand *cmd = new ScaleImageCommand(before, newSize, mModel);
    mUndoStack->push(cmd);
}



