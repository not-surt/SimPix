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
    Q_PROPERTY(Image *image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Image *image() const;

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
    void license();
    void setImage(Image *image);

protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    Image *m_image;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
