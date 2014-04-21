#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void closeEvent(QCloseEvent *event);
signals:
    void imageChanged(Image *);

public slots:
    void newImage();
    void openImage();
    void saveImage();
    void saveAsImage();
    void setFullscreen(bool);
    void about();
    void aboutQt();
    void closeFile();
private:
    Ui::MainWindow *ui;
    Image *image;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
