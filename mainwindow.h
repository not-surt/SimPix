#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include "image.h"
#include "imageeditor.h"
#include "statusmousewidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    void showToolbars(bool checked);
    void showDocks(bool checked);
    void showDockTitles(bool checked);
    void lockSubwindows(bool checked);
    ImageEditor *newEditor(Image *const image);
    void activateSubWindow(QMdiSubWindow *const subWindow);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QMdiArea *m_mdi;
    StatusMouseWidget *m_statusMouseWidget;
    QMdiSubWindow *m_oldSubWindow;
    QList<Image *> m_images;
    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
