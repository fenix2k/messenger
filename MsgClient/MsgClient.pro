#-------------------------------------------------
#
# Project created by QtCreator 2016-07-24T11:39:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MsgClient
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    logindialog.cpp \
    netclient.cpp

HEADERS  += \
    mainwindow.h \
    logindialog.h \
    netclient.h

FORMS    += \
    logindialog.ui \
    mainwindow.ui
