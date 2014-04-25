#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoStack>
#include "image.h"

namespace Ui {
class MainWindow;
}

class StrokeCommand : public QUndoCommand
{
public:
    StrokeCommand(const Image &image, const QPoint &a, const QPoint &b, QUndoCommand *parent = 0);
    ~StrokeCommand();

    void undo();
    void redo();

private:
    QPointF a, b;
    const Image &image;
    Image dirty;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image NOTIFY imageChanged)

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
    void stroke(const QPoint &a, const QPoint &b);
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    Image *m_image;
    QUndoStack *undoStack;

    static const QString fileDialogFilterString;
};

#endif // MAINWINDOW_H
