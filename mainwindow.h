#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


signals:
    void imageChanged(QImage *);

public slots:
    void newImage();
    void openImage();
    void saveImage();
    void saveAsImage();

private:
    Ui::MainWindow *ui;
    QImage *image;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
