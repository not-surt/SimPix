#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "mainwindow.h"

class Application : public QApplication
{
public:
    Application(int &argc, char **argv);
    static Application *instance();
private:
    MainWindow window;
};

#endif // APPLICATION_H
