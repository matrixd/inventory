#-------------------------------------------------
#
# Project created by QtCreator 2015-04-29T14:00:49
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = inventory
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui \
    mainwindow.ui

CONFIG += static
CONFIG += static-libgcc
