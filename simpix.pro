#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T09:55:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simpix
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newdialog.cpp \
    canvas.cpp \
    colourselector.cpp \
    transformwidget.cpp \
    palettemodel.cpp \
    palettewidget.cpp \
    paletteview.cpp \
    colourdelegate.cpp \
    image.cpp

HEADERS  += mainwindow.h \
    newdialog.h \
    canvas.h \
    colourselector.h \
    transformwidget.h \
    constants.h \
    palettemodel.h \
    palettewidget.h \
    paletteview.h \
    colourdelegate.h \
    image.h

FORMS    += mainwindow.ui \
    newdialog.ui \
    colourselector.ui \
    transformwidget.ui \
    palettewidget.ui

QMAKE_CXXFLAGS += -std=c++11
