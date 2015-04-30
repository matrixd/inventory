#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QStandardItemModel>
#include <QStandardItem>

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
    void readAll(QString fname);
private slots:
    void add();
    void saveAll();
    void proccessItem(QStandardItem* it);
    void checkItem(QStandardItem* it);
    void print();
};

#endif // MAINWINDOW_H
