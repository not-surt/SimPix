#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T09:55:43
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simpix
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    newdialog.cpp \
    colourselector.cpp \
    transformwidget.cpp \
    palettemodel.cpp \
    palettewidget.cpp \
    paletteview.cpp \
    paletteplugin.cpp \
    transform.cpp \
    colourcontextwidget.cpp \
    colourswatch.cpp \
    colourswatchdelegate.cpp \
    util.cpp \
    application.cpp \
    statusmousewidget.cpp \
    openglwindow.cpp \
    scene.cpp \
    scenewindow.cpp \
    editingcontext.cpp

HEADERS  += mainwindow.h \
    newdialog.h \
    colourselector.h \
    transformwidget.h \
    palettemodel.h \
    palettewidget.h \
    paletteview.h \
    paletteplugin.h \
    transform.h \
    colourcontextwidget.h \
    colourswatchdelegate.h \
    colourswatch.h \
    util.h \
    application.h \
    statusmousewidget.h \
    openglwindow.h \
    scene.h \
    scenewindow.h \
    editingcontext.h

FORMS    += mainwindow.ui \
    newdialog.ui \
    colourselector.ui \
    transformwidget.ui \
    palettewidget.ui \
    colourcontextwidget.ui \
    statusmousewidget.ui

CONFIG += c++11

OTHER_FILES += \
    paletteplugin.json \
    simpix.rc \
    images/simpix-48x48.png \
    images/simpix.ico \
    text/LICENSE \
    image.vert \
    shaders/image.frag \
    shaders/frame.frag \
    shaders/checkerboard.frag \
    shaders/canvas.vert \
    shaders/ellipsebrush.frag \
    shaders/rectanglebrush.frag \
    shaders/brush.vert

RESOURCES += \
    simpix.qrc

RC_FILE = simpix.rc
