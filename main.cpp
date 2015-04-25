#include "application.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

    // Prevent moving toolbars/docks killing QOpenGLWidgets in linux BUT menus don't appear in fullscreen in windows.
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);

    return Application(argc, argv).exec();
}
