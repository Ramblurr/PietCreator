#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>

class QSpinBox;

class ResizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResizeDialog( const QSize &size, QWidget *parent = 0);
    ~ResizeDialog();

    QSize newSize() const;

private:
    QSpinBox* mWidthSpin;
    QSpinBox* mHeightSpin;
};

#endif // RESIZEDIALOG_H
