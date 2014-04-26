#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("SimPix");
    a.setOrganizationName("Uninhabitant");
    a.setOrganizationDomain("uninhabitant.com");

//    a.setStyleSheet(
//        "* {color: #fff; background-color: #000}"
//        "* {font-size: 24pt}"
//        "* {icon-size: 48px}"
//        );

    MainWindow w;
    w.show();

    return a.exec();
}
