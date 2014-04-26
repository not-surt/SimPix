#include "application.h"
#include "mainwindow.h"
#include <QDebug>

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), window()
{
    setApplicationName("SimPix");
    setOrganizationName("Uninhabitant");
    setOrganizationDomain("uninhabitant.com");

    setStyleSheet(
//        "* {color: #fff; background-color: #000}"
//        "* {font-size: 24pt}"
//        "* {icon-size: 48px}"
//        "* {margin: 0px; border: 0px; padding: 0px}"
        ""
        );

    window.show();
}

Application *Application::instance()
{
    return (Application *)QApplication::instance();
}
