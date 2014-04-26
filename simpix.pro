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
    image.cpp \
    paletteplugin.cpp \
    transform.cpp \
    colourcontextwidget.cpp \
    colourswatch.cpp \
    colourswatchdelegate.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    newdialog.h \
    canvas.h \
    colourselector.h \
    transformwidget.h \
    palettemodel.h \
    palettewidget.h \
    paletteview.h \
    image.h \
    paletteplugin.h \
    transform.h \
    colourcontextwidget.h \
    colourswatchdelegate.h \
    colourswatch.h \
    util.h

FORMS    += mainwindow.ui \
    newdialog.ui \
    colourselector.ui \
    transformwidget.ui \
    palettewidget.ui \
    colourcontextwidget.ui

#QMAKE_CXXFLAGS += -std=c++11
CONFIG += c++11

include(color_widgets/color_widgets.pri)

OTHER_FILES += \
    paletteplugin.json
