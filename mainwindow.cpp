#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QFile>
#include <QByteArray>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    items = new QList<Item>();
    itemsModel = new QStandardItemModel(0,2,this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addItem(QString item)
{
    QStandardItem* it = new QStandardItem(item);
    itemsModel->appendRow(it);
}

void MainWindow::readAll(QString fname)
{
    QFile fl(fname, this);
    QByteArray str = fl.readLine();
    while(!fl.atEnd()){
        addItem(QString(str));
    }
}

void MainWindow::generateTable()
{
    for(QList<Item>::iterator it = items->begin(); it != items->end(); it++){

    }
}

