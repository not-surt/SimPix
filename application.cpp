#include "application.h"
#include "mainwindow.h"
#include <QDebug>
#include "canvaswindow.h"

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), window()
{
    setWindowIcon(QIcon("://images/simpix-48x48.png"));

    setApplicationName("SimPix");
    setOrganizationName("Uninhabitant");
    setOrganizationDomain("uninhabitant.com");

    setStyleSheet(
        "QStatusBar::item {border: none}"
//        "* {color: #fff; background-color: #000}"
//        "* {font-size: 24pt}"
//        "* {icon-size: 48px}"
//        "* {margin: 0px; border: 0px; padding: 0px}"
        ""
        );

    window.show();

//    CanvasWindow *window = new CanvasWindow();
//    QWidget *widget = QWidget::createWindowContainer(window);
//    widget->show();
}

Application *Application::instance()
{
    return (Application *)QApplication::instance();
}
