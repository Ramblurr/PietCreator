#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

#include <QColor>
#include <QImage>

class ImageModel;
class QUndoStack;

class UndoHandler
{

public:
    UndoHandler(QUndoStack * undostack, ImageModel* model);

    void createEditPixel(int x, int y, QColor new_color, bool dragging = false);
    void insertImage(int x, int y, QImage imageToInsert, QSize scaleAfter);
    void scaleImage(QSize newSize);

private:
    ImageModel* mModel;
    QUndoStack* mUndoStack;
};

#endif // ACTIONHANDLER_H
