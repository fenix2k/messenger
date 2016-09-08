#-------------------------------------------------
#
# Project created by QtCreator 2016-07-24T19:23:11
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MsgServer
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    myclient.cpp \
    myserver.cpp

HEADERS  += \
    mainwindow.h \
    myclient.h \
    myserver.h

FORMS    += \
    mainwindow.ui
