#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QTextDocument>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lstModel = new QStandardItemModel(0,2,this);
    QStandardItem* it = new QStandardItem("Наименование");
    lstModel->setHorizontalHeaderItem(0,it);
    it = new QStandardItem("Страница");
    lstModel->setHorizontalHeaderItem(1,it);
    ui->tableView->setModel(lstModel);

    itemsModel = new QStandardItemModel(0,2,this);
    it = new QStandardItem("Наименование");
    lstModel->setHorizontalHeaderItem(0,it);
    it = new QStandardItem("Страница");
    lstModel->setHorizontalHeaderItem(1,it);
    ui->tableView_2->setModel(itemsModel);

    readAll("lst.txt");

    connect(lstModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(proccessItem(QStandardItem*)));
    connect(itemsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(checkItem(QStandardItem*)));
    connect(ui->printButton, SIGNAL(clicked()), this, SLOT(print()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveAll()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(add()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addToLst(QString item)
{
    QStandardItem* it = new QStandardItem(item);
    lstModel->appendRow(it);
}

void MainWindow::readAll(QString fname)
{
    QFile fl(fname, this);
    if(!fl.open(QFile::ReadOnly)){
        return;
    }

    while((!fl.atEnd())){
        QByteArray str = fl.readLine();
        addToLst(QString(str));
    }
    fl.close();
}

void MainWindow::saveAll()
{
    QFile fl("lst.txt");
    if(!fl.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    for(int row = 0; row != itemsModel->rowCount(); row++)
        fl.write(itemsModel->item(row)->text().toUtf8());

    fl.close();
}

void MainWindow::add()
{
    addToLst(ui->lineEdit->text());
}

void MainWindow::proccessItem(QStandardItem *it)
{
    if(it->column() == 0){

    }
    if(it->column() == 1){
        QList<QStandardItem*> lst = itemsModel->findItems(lstModel->item(it->row())->text());
        if(it->text().toInt() > 0){
            if(lst.size() < 1){
                QStandardItem* newitem = new QStandardItem(lstModel->item(it->row())->text());
                itemsModel->appendRow(newitem);
                int row = newitem->row();
                //newitem = new QStandardItem(it->text());
                newitem = new QStandardItem();
                newitem->setData(QVariant(it->text().toInt()), Qt::EditRole);
                itemsModel->setItem(row,1,newitem);
            } else
                itemsModel->item(lst.at(0)->row(),1)->setData(QVariant(it->text().toInt()), Qt::EditRole);

            itemsModel->setSortRole(Qt::EditRole);
            itemsModel->sort(1);
        } else {
            if(lst.size() > 0){
                itemsModel->removeRow(lst.at(0)->row());
            }
        }
    }
}

void MainWindow::checkItem(QStandardItem *it)
{
    if(it->column() < 1)
        return;

    if(it->text().toInt() < 1)
        itemsModel->removeRow(it->row());
    else
        it->setData(QVariant(it->text().toInt()), Qt::EditRole);
}

void MainWindow::print()
{
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    qDebug() << "print";
    QString html = "<html>\n<head><style type=\"text/css\">body{font-size: 18px;}</style></head>\n<body>\n";
    html += "<h2>Внутренняя опись</h2>\n<br>\n";
    html += "<table cellspacing=0 cellpadding = 2 style='border-width: 1px; border-style: solid; border-color: #000000'>\n";
    /*QList<QStandardItem*> lst = itemsModel->findItems("\*",Qt::MatchRegExp);
    qDebug() << lst.size();
    for(QList<QStandardItem*>::iterator it = lst.begin(); it != lst.end(); it++){
        qDebug() << (*it)->text();
    }*/
    for(int row = 0; row != itemsModel->rowCount(); row++){
        html += "    <tr><td>";
        html += itemsModel->item(row)->text().remove("\n");
        html += "</td><td>";
        html += itemsModel->item(row,1)->text();
        html += "</td></tr>\n";
    }
    html += "</table>\n</body>\n</html>";
    qDebug() << html;
    QTextDocument *document = new QTextDocument();
    document->setHtml(html);
    document->print(&printer);
}
