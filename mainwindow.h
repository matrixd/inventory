#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QStandardItemModel>

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
    class Item{
    public:
        unsigned int page;
        QString name;
    };

    Ui::MainWindow *ui;
    QList<Item>* items;
    QStandardItemModel* itemsModel;

    void addItem(QString it);
    void readAll(QString fname);
private slots:
    void generateTable();
};

#endif // MAINWINDOW_H
