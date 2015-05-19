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
#include <QScrollBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lstModel = new QStandardItemModel(0,3,this);
    QStandardItem* it = new QStandardItem("Наименование");
    lstModel->setHorizontalHeaderItem(0,it);
    it = new QStandardItem("Страница(с)");
    lstModel->setHorizontalHeaderItem(1,it);
    it = new QStandardItem("Страница(до)");
    lstModel->setHorizontalHeaderItem(2,it);
    ui->tableView->setModel(lstModel);

    itemsModel = new QStandardItemModel(0,3,this);
    it = new QStandardItem("Наименование");
    itemsModel->setHorizontalHeaderItem(0,it);
    it = new QStandardItem("Страница");
    itemsModel->setHorizontalHeaderItem(1,it);
    it = new QStandardItem("Страница(до)");
    itemsModel->setHorizontalHeaderItem(2,it);
    ui->tableView_2->setModel(itemsModel);
    itemsModel->setSortRole(Qt::EditRole);

    readAll("lst.txt");

    connect(lstModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(proccessItem(QStandardItem*)));
    connect(itemsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(checkItem(QStandardItem*)));
    connect(ui->printButton, SIGNAL(clicked()), this, SLOT(print()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveAll()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(add()));
    connect(ui->plusButton, SIGNAL(clicked()), this, SLOT(plus()));
    connect(ui->clrButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));
    connect(ui->tableView->verticalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(moveItem(int,int,int)));

    ui->tableView->verticalHeader()->setSectionsMovable(1);
    ui->tableView->verticalHeader()->setUpdatesEnabled(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addToLst(QString item)
{
    QStandardItem* it = new QStandardItem(item);
    //lstModel->setItem();
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
        addToLst(QString(str).remove("\n"));
    }
    fl.close();
}

void MainWindow::saveAll()
{
    QFile fl("lst.txt");
    if(!fl.open(QFile::WriteOnly))
        return;

    for(int row = 0; row < lstModel->rowCount(); row++)
        fl.write(lstModel->item(row)->text().toUtf8()+'\n');

    fl.close();
}

void MainWindow::add()
{
    addToLst(ui->lineEdit->text());
    ui->tableView->selectRow(lstModel->rowCount()-1);
}

void MainWindow::proccessItem(QStandardItem *it)
{
    if(it->column() == 0){
        return;
    }
    if(it->column() > 0){
        QList<QStandardItem*> lst = itemsModel->findItems(lstModel->item(it->row())->text());
        if(it->text().toInt() > 0){
            if(lst.size() < 1){
                //hack
                itemsModel->blockSignals(1);
                QStandardItem* newitem = new QStandardItem(lstModel->item(it->row(),0)->text());
                itemsModel->appendRow(newitem);
                int row = newitem->row();
                qDebug() << newitem->column();
                //newitem = new QStandardItem(it->text());
                itemsModel->setItem(row,0,newitem);
                newitem = new QStandardItem();
                if(lstModel->item(it->row(),1))
                    newitem->setData(QVariant(lstModel->item(it->row(),1)->text().toInt()),
                                     Qt::EditRole);
                else
                    newitem->setData(QVariant(0),
                                     Qt::EditRole);

                itemsModel->setItem(row,1,newitem);
                newitem = new QStandardItem();
                if(lstModel->item(it->row(),2))
                    newitem->setData(QVariant(lstModel->item(it->row(),2)->text().toInt()),
                                     Qt::EditRole);
                else
                    newitem->setData(QVariant(0),
                                     Qt::EditRole);

                itemsModel->setItem(row,2,newitem);
            } else {
                if(lstModel->item(it->row(),1))
                    itemsModel->item(lst.at(0)->row(),1)->setData(QVariant(lstModel->item(it->row(),1)->text().toInt()), Qt::EditRole);
                if(lstModel->item(it->row(),2))
                    itemsModel->item(lst.at(0)->row(),2)->setData(QVariant(lstModel->item(it->row(),2)->text().toInt()), Qt::EditRole);
            }
            itemsModel->blockSignals(0);
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

    if(it->text().toInt() < 1 && it->column() == 1)
        itemsModel->removeRow(it->row());
    else
        it->setData(QVariant(it->text().toInt()), Qt::EditRole);

    itemsModel->sort(1);
}

void MainWindow::print()
{
    QFile begin("begin.html");
    QFile end("end.html");

    if(!begin.open(QFile::ReadOnly) || !end.open(QFile::ReadOnly))
        return;

    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString html;
    html = begin.readAll();
    begin.close();
    for(int row = 0; row != itemsModel->rowCount(); row++){
        html += "    <tr><td>";
        html += itemsModel->item(row)->text();
        html += "</td><td>";
        html += itemsModel->item(row,1)->text();
        if(itemsModel->item(row,2)->text().toInt() > 0)
            html += '-'+itemsModel->item(row,2)->text();
        html += "</td></tr>\n";
    }
    html += end.readAll();
    end.close();
    qDebug() << html;
    QTextDocument *document = new QTextDocument();
    document->setHtml(html);
    document->print(&printer);
}

void MainWindow::plus()
{
    int srow = -1;
    qDebug() << ui->tableView->selectionModel()->selectedRows().length();
    if(ui->tableView->selectionModel()->selectedRows().length())
        srow = ui->tableView->selectionModel()->selectedRows().at(0).row();
    if(srow<0)
        return;
    itemsModel->blockSignals(1);
    QStandardItem* newitem = new QStandardItem(lstModel->item(srow)->text());
    itemsModel->appendRow(newitem);
    int row = newitem->row();
    newitem = new QStandardItem();
    if(row > 0)
        newitem->setData(QVariant(itemsModel->item(row-1,1)->data(Qt::EditRole).toInt()+1),
                         Qt::EditRole);
    else
        newitem->setData(QVariant(1),
                             Qt::EditRole);
    itemsModel->setItem(row,1,newitem);
    newitem = new QStandardItem();
    newitem->setData(QVariant(0),
                         Qt::EditRole);
    itemsModel->setItem(row,2,newitem);
    itemsModel->blockSignals(0);
    itemsModel->sort(1);
}

void MainWindow::clear()
{
    QMessageBox msgBox;
    msgBox.setText("Cleat");
    msgBox.setInformativeText("Do you really want to clear your table?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if(QMessageBox::No == msgBox.exec())
        return;
    itemsModel->clear();
    lstModel->blockSignals(1);
    for(int row = 0; row != lstModel->rowCount(); row++){
        QStandardItem* it = new QStandardItem("0");
        lstModel->setItem(row,1,it);
        it = new QStandardItem("0");
        lstModel->setItem(row,2,it);
    }
    lstModel->blockSignals(0);
}

void MainWindow::search(QString qstr)
{
    QList<QStandardItem*> lst = lstModel->findItems(qstr, Qt::MatchStartsWith);
    if(lst.size() < 1)
        return;
    ui->tableView->selectRow(lst.at(0)->row());
}

void MainWindow::moveItem(int logical, int old, int n)
{
    lstModel->insertRow(n,lstModel->takeRow(old));
}
