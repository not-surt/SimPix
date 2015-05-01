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
    newdialog.cpp \
    transformwidget.cpp \
    transform.cpp \
    colourcontextwidget.cpp \
    colourswatchdelegate.cpp \
    util.cpp \
    application.cpp \
    statusmousewidget.cpp \
    editingcontext.cpp \
    imageeditor.cpp \
    session.cpp \
    document.cpp \
    data.cpp \
    editor.cpp \
    imagedocument.cpp \
    widgets.cpp \
    actions.cpp \
    sessionwidget.cpp \
    palettewidget.cpp \
    treemodel.cpp \
    colourselectorwidget.cpp \
    colourswatchwidget.cpp \
    settingsdialog.cpp \
    window.cpp

HEADERS  += \
    newdialog.h \
    transformwidget.h \
    transform.h \
    colourcontextwidget.h \
    colourswatchdelegate.h \
    util.h \
    application.h \
    statusmousewidget.h \
    editingcontext.h \
    imageeditor.h \
    session.h \
    document.h \
    data.h \
    editor.h \
    imagedocument.h \
    widgets.h \
    actions.h \
    sessionwidget.h \
    palettewidget.h \
    treemodel.h \
    stable.h \
    colourselectorwidget.h \
    colourswatchwidget.h \
    settingsdialog.h \
    window.h

PRECOMPILED_HEADER = stable.h

FORMS    += mainwindow.ui \
    newdialog.ui \
    transformwidget.ui \
    palettewidget.ui \
    colourcontextwidget.ui \
    statusmousewidget.ui \
    sessionwidget.ui \
    colourselectorwidget.ui

CONFIG += c++11

OTHER_FILES += \
    simpix.rc \
    images/simpix-48x48.png \
    images/simpix.ico \
    text/LICENSE \
    shaders/image.frag \
    shaders/frame.frag \
    shaders/checkerboard.frag \
    shaders/brush.vert \
    text/ABOUT \
    shaders/viewport.vert \
    shaders/brushellipse.frag \
    shaders/brushrectangle.frag \
    shaders/canvassingle.vert \
    shaders/canvastiled.vert

RESOURCES += \
    simpix.qrc

RC_FILE = simpix.rc
