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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Image *image() const;

signals:
    void imageChanged(Image *const);

public slots:
    bool newImage();
    bool openImage();
    bool saveImage();
    bool saveAsImage();
    void setFullscreen(bool);
    void about();
    void aboutQt();
    bool closeImage(const bool doClose = true);
    void exit();
    void license();
    void setImage(Image *image = nullptr);

protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    Image *m_image;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
