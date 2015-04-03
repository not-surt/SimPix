#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include "image.h"
#include "imageeditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Image *image() const;

signals:
    void sceneChanged(Image *const image);

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
    void setImageEditor(Image *image = nullptr, ImageEditor *editor = nullptr);
    void showToolbars(bool checked);
    void showDocks(bool checked);
    void showDockTitles(bool checked);
    void lockSubwindows(bool checked);
    ImageEditor *newEditor(Image *const image = nullptr);

protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    Image *m_image;
    ImageEditor *m_imageEditor;
    QMdiArea *m_mdi;
    QList<Image *> m_images;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
