#-------------------------------------------------
#
# Project created by QtCreator 2014-11-12T14:19:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AndroidStrings
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    androidstring.cpp \
    androidstringreader.cpp \
    androidstringmodel.cpp \
    processingdialog.cpp

HEADERS  += mainwindow.h \
    androidstring.h \
    androidstringreader.h \
    androidstringmodel.h \
    processingdialog.h

FORMS    += mainwindow.ui \
    processingdialog.ui
