#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include "document.h"
#include "imageeditor.h"
#include "statusmousewidget.h"

namespace Ui {
class MainWindow;
}

class SubWindow : public QMdiSubWindow
{
public:
    explicit SubWindow(QWidget *parent = nullptr) :
        QMdiSubWindow(parent) {}

protected:
    void closeEvent(QCloseEvent *event);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void newImage();
    void openImage();
    bool saveImage();
    bool saveAsImage();
    void setFullscreen(bool);
    void about();
    void aboutQt();
    bool closeImage();
    void license();
    void showToolbars(bool checked);
    void lockToolbars(bool checked);
    void showDocks(bool checked);
    void showDockTitles(bool checked);
    void lockDocks(bool checked);
    void useTabs(bool checked);
    ImageEditor *newEditor(ImageDocument *const image);
    void activateSubWindow(QMdiSubWindow *const subWindow);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QMdiArea *m_mdi;
    StatusMouseWidget *m_statusMouseWidget;
    QMdiSubWindow *m_oldSubWindow;
    QList<ImageDocument *> m_images;
    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
