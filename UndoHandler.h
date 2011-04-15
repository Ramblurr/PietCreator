#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

#include <QColor>

class ImageModel;
class QUndoStack;

class UndoHandler
{

public:
    UndoHandler(QUndoStack * undostack, ImageModel* model);

    void createEditPixel(int x, int y, QColor new_color);
private:
    ImageModel* mModel;
    QUndoStack* mUndoStack;
};

#endif // ACTIONHANDLER_H
