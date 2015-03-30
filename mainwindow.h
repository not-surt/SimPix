#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scene.h"
#include "canvaswidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Scene *scene() const;

signals:
    void sceneChanged(Scene *const scene);

public slots:
    bool newScene();
    bool openScene();
    bool saveScene();
    bool saveAsScene();
    void setFullscreen(bool);
    void about();
    void aboutQt();
    bool closeScene(const bool doClose = true);
    void license();
    void setScene(Scene *scene = nullptr);

protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    Scene *m_scene;
    CanvasWidget *m_canvas;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
