#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scenewindow.h"
#include "scene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(Scene *image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Scene *image() const;

signals:
    void imageChanged(Scene *const);

public slots:
    bool newImage();
    bool openImage();
    bool saveImage();
    bool saveAsImage();
    void setFullscreen(bool);
    void about();
    void aboutQt();
    bool closeImage(const bool doClose = true);
    void license();
    void setImage(Scene *image = nullptr);

protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    Scene *m_image;
    SceneWindow *canvas;
//    Canvas *canvas;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
