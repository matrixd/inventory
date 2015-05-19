#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTextStream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* lstModel;
    QStandardItemModel* itemsModel;

    void addToLst(QString it);
    //void addToLst(QString root, QString node);
    void readAll(QString fname);
    void writeTree(QTextStream *out, QStandardItem* root, int lvl);
    void readTree(QTextStream *in, QStandardItem* root, int lvl);
private slots:
    void add();
    void saveAll();
    void proccessItem(QStandardItem* it);
    void checkItem(QStandardItem* it);
    void print();
    void plus();
    void clear();
    void search(QString qstr);
    void moveItem(int logical, int old, int n);
};

#endif // MAINWINDOW_H
