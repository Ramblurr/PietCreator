#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QTableWidget>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTableWidget* table = new QTableWidget(this);
    table->setColumnCount(50);
    table->setRowCount(50);
    table->horizontalHeader()->setDefaultSectionSize(20);
    table->verticalHeader()->setDefaultSectionSize(20);
    table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    setCentralWidget(table);

}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "MainWindow.moc"