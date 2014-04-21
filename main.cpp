#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("SimPix");
    a.setOrganizationName("Uninhabitant");
    a.setOrganizationDomain("uninhabitant.com");

    MainWindow w;
    w.show();

    return a.exec();
}
