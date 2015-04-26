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
    explicit SubWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event);
};

class MdiArea : public QMdiArea
{
public:
    explicit MdiArea(QWidget *parent = nullptr) :
        QMdiArea(parent) {}
    QSize subWindowSizeOverhead() const;
};

class PaletteWidget;
class ColourSelector;
class ColourContextWidget;
class SessionWidget;
class StatusMouseWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MdiArea *mdi;
    PaletteWidget *paletteWidget;
    ColourSelector *colourSelector;
    ColourContextWidget *colourContextWidget;
    SessionWidget *sessionWidget;
    StatusMouseWidget *statusMouseWidget;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Editor *activeEditor();

public slots:
    void setFullscreen(bool);
    void showToolbars(bool checked);
    void lockToolbars(bool checked);
    void showDocks(bool checked);
    void showDockTitles(bool checked);
    void lockDocks(bool checked);
    void useTabs(bool checked);
    SubWindow *newEditorSubWindow(ImageEditor *const editor);
    void activateSubWindow(SubWindow *const subWindow);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    SubWindow *oldSubWindow;
    static const QString fileDialogFilterString;
    QList<QMetaObject::Connection> activeSubWindowConnections;
};

#endif // MAINWINDOW_H
