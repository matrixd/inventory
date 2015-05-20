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
#include <QStringRef>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lstModel = new QStandardItemModel(0,1,this);
    lstSelModel = new QItemSelectionModel(lstModel);
    ui->treeView->setModel(lstModel);
    //ui->treeView->sc
    ui->treeView->setAutoExpandDelay(0);

    itemsModel = new QStandardItemModel(0,3,this);
    QStandardItem *it = new QStandardItem("Наименование");
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStandardItem* MainWindow::addToLst(QString item)
{
    QStandardItem* it = new QStandardItem(item);
    lstModel->appendRow(it);
    return it;
}

void MainWindow::readAll(QString fname)
{
    QFile fl(fname, this);
    if(!fl.open(QFile::ReadOnly)){
        return;
    }

    QTextStream in(&fl);
    while(!in.atEnd()){
        treeRead(&in,0,0);
    }
    fl.close();
}

void MainWindow::treeWrite(QTextStream *out, QStandardItem *root, int lvl)
{
    for(int l = lvl; l; l--)
        *out << "    ";
    *out << root->text() << "\n";
    qDebug() << out;
    if(root->hasChildren()){
        for(int crow = 0; crow < root->rowCount(); crow++)
            treeWrite(out, root->child(crow), lvl+1);
    }
}

void MainWindow::treeRead(QTextStream *in, QStandardItem *root, int lvl)
{
    if(in->atEnd())
        return;
    QString str = in->readLine();
    QStringRef ref(&str);
    if(ref.count("    ") > lvl){
        QStandardItem *it = new QStandardItem(str.remove("\n").remove("    "));
        root->appendRow(it);
        treeRead(in, it, ref.count("    "));
        return;
    }
    if(ref.count("    ") == 0){
        QStandardItem *it = new QStandardItem(str.remove("\n"));
        lstModel->appendRow(it);
        treeRead(in, it, 0);
        return;
    }
    if(ref.count("    ") == lvl){
        QStandardItem *it = new QStandardItem(str.remove("\n").remove("    "));
        root->appendRow(it);
        treeRead(in, root, lvl);
        return;
    }
}


void MainWindow::saveAll()
{
    QFile fl("lst.txt");
    if(!fl.open(QFile::WriteOnly))
        return;

    QTextStream out(&fl);
    for(int row = 0; row < lstModel->rowCount(); row++){
        treeWrite(&out, lstModel->item(row),0);
    }

    fl.close();
}

void MainWindow::add()
{
    QStandardItem* it = addToLst(ui->lineEdit->text());
    ui->treeView->scrollTo(it->index());
    ui->treeView->selectionModel()->select(it->index(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
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
    QModelIndex srow;
    if(ui->treeView->selectionModel()->selectedIndexes().length())
        srow = ui->treeView->selectionModel()->selectedIndexes().at(0);
    //if(srow)
    //    return;
    qDebug() << srow;
    itemsModel->blockSignals(1);
    QStandardItem* newitem = new QStandardItem(srow.data().toString());
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
}

void MainWindow::search(QString qstr)
{
    if(qstr.count() < 1)
        return;
    QList<QStandardItem*> lst = lstModel->findItems(qstr, Qt::MatchStartsWith | Qt::MatchRecursive);
    if(lst.size() < 1)
        return;
    qDebug() << "1";
    ui->treeView->scrollTo(lst.at(0)->index());
    ui->treeView->selectionModel()->select(lst.at(0)->index(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void MainWindow::moveItem(int logical, int old, int n)
{
    lstModel->insertRow(n,lstModel->takeRow(old));
}
