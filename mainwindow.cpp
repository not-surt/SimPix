#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "util.h"
#include "colourswatch.h"
#include "statusmousewidget.h"
#include "application.h"
#include "canvaswidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QMdiArea>
#include <QTextStream>

const QString MainWindow::fileDialogFilterString = tr("PNG Image Files (*.png)");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_scene(nullptr), m_canvas(nullptr)
{    
    ui->setupUi(this);

    m_canvas = new CanvasWidget();
    setCentralWidget(m_canvas);

    // Copy actions to window. Is there a better way?
    QList<QMenu *> menus = ui->menuBar->findChildren<QMenu *>();
    QListIterator<QMenu *> menu(menus);
    while (menu.hasNext()) {
        addActions(menu.next()->findChildren<QAction *>());
    }

    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newScene()));
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openScene()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveScene()));
    QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsScene()));
    QObject::connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeScene()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionMenu, SIGNAL(triggered(bool)), this->menuBar(), SLOT(setVisible(bool)));
    QObject::connect(ui->actionStatusBar, SIGNAL(triggered(bool)), this->statusBar(), SLOT(setVisible(bool)));
    QObject::connect(ui->actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(setFullscreen(bool)));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    QObject::connect(ui->actionLicense, SIGNAL(triggered()), this, SLOT(license()));

    QObject::connect(ui->paletteWidget, SIGNAL(colourChanged(const uint)), m_canvas, SLOT(update()));

    QObject::connect(ui->transformWidget, SIGNAL(transformChanged(Transform)), m_canvas, SLOT(setTransform(Transform)));
    QObject::connect(m_canvas, SIGNAL(transformChanged(Transform)), ui->transformWidget, SLOT(setTransform(Transform)));

    QObject::connect(ui->actionTiled, SIGNAL(triggered(bool)), m_canvas, SLOT(setTiled(bool)));
    QObject::connect(ui->actionShowFrame, SIGNAL(triggered(bool)), m_canvas, SLOT(setShowFrame(bool)));
    QObject::connect(ui->actionAlpha, SIGNAL(triggered(bool)), m_canvas, SLOT(setShowAlpha(bool)));

    QMenu *toolBarMenu = new QMenu(this);
    ui->actionToolbars->setMenu(toolBarMenu);
    QList<QToolBar *> toolbars = findChildren<QToolBar *>();
    QListIterator<QToolBar *> toolbar(toolbars);
    while (toolbar.hasNext()) {
        toolBarMenu->addAction(toolbar.next()->toggleViewAction());
    }
//    {
//        QListIterator<QToolBar *> toolbar(toolbars);
//        while (toolbar.hasNext()) {
//            toolbar.next()->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
//        }
//    }

    QMenu *dockMenu = new QMenu(this);
    ui->actionDocks->setMenu(dockMenu);
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    QListIterator<QDockWidget *> dock(docks);
    while (dock.hasNext()) {
        dockMenu->addAction(dock.next()->toggleViewAction());
    }
    StatusMouseWidget *statusMouseWidget = new StatusMouseWidget();
    statusMouseWidget->hide();
    statusBar()->addWidget(statusMouseWidget);
    QObject::connect(m_canvas, SIGNAL(mouseEntered()), statusMouseWidget, SLOT(show()));
    QObject::connect(m_canvas, SIGNAL(mouseLeft()), statusMouseWidget, SLOT(hide()));
    QObject::connect(m_canvas, SIGNAL(mousePixelChanged(QPoint, QColor, int)), statusMouseWidget, SLOT(setMouseInfo(QPoint, QColor, int)));

    QSettings settings;
    settings.beginGroup("window");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
//    toolbars = findChildren<QToolBar *>();
//    toolbar = QListIterator<QToolBar *>(toolbars);
//    while (toolbar.hasNext()) {
//        toolbar.next()->restoreGeometry(settings.value(QString("window/%1geometry").arg(toolbar.objectName())).toByteArray());
//    }

    m_canvas->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_scene;
}

Scene *MainWindow::scene() const
{
    return m_scene;
}

void MainWindow::setScene(Scene *scene)
{
    if (m_scene != scene) {
        if (scene) {
            QObject::connect(scene, SIGNAL(changed(const QRegion &)), m_canvas, SLOT(update()));
//            QObject::connect(m_canvas->editingContext(), SIGNAL(changed(EditingContext *)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::connect(m_canvas, SIGNAL(clicked(const QPoint &, EditingContext *const)), scene, SLOT(point(const QPoint &, EditingContext *const)));
            QObject::connect(m_canvas, SIGNAL(dragged(const QPoint &, const QPoint &, EditingContext *const)), scene, SLOT(stroke(const QPoint &, const QPoint &, EditingContext *const)));
            setWindowFilePath(scene->fileName());
        }
        else {
            setWindowFilePath(QString());
        }
        emit sceneChanged(scene);
        if (m_scene) {
            QObject::disconnect(m_scene, SIGNAL(changed(const QRegion &)), m_canvas, SLOT(update()));
//            QObject::disconnect(m_scene, SIGNAL(contextColourChanged(const uint, const int)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::disconnect(m_canvas, SIGNAL(clicked(const QPoint &, EditingContext *const)), m_scene, SLOT(point(const QPoint &, EditingContext *const)));
            QObject::disconnect(m_canvas, SIGNAL(dragged(const QPoint &, const QPoint &, EditingContext *const)), m_scene, SLOT(stroke(const QPoint &, const QPoint &, EditingContext *const)));
            m_scene->deleteLater();
        }
        m_scene = scene;
        ui->paletteWidget->setEditingContext(nullptr);
        m_canvas->setScene(m_scene);
        ui->paletteWidget->setEditingContext(&m_canvas->editingContext());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!closeScene()) {
        event->ignore();
    }
    else {
        QSettings settings;
        settings.beginGroup("window");
        settings.setValue("geometry", saveGeometry());
        settings.setValue("state", saveState());
        settings.endGroup();
        QMainWindow::closeEvent(event);
    }
}

bool MainWindow::newScene()
{
    if (!closeScene(false)) {
        return false;
    }
    NewDialog *dialog = new NewDialog(this);
    if (dialog->exec()) {
        Scene *newImage = new Scene(dialog->imageSize(), dialog->mode());
        setScene(newImage);
        return true;
    }
    return false;
}

bool MainWindow::openScene()
{
    if (!closeScene(false)) {
        return false;
    }
    QSettings settings;
    settings.beginGroup("file");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), settings.value("lastOpened", QDir::homePath()).toString(), fileDialogFilterString);
    if (!fileName.isNull()) {
        settings.setValue("lastOpened", fileName);
        Scene *newImage = new Scene(fileName);
        if (!newImage->imageData()) {
            delete newImage;
            QMessageBox::critical(this, QString(), QString(tr("Error opening file <b>\"%1\"</b>")).arg(QFileInfo(fileName).fileName()));
        }
        else {
            setScene(newImage);
            return true;
        }
    }
    settings.endGroup();
    return false;
}

bool MainWindow::saveScene()
{
    if (m_scene) {
        QSettings settings;
        settings.beginGroup("file");
        if (m_scene->fileName().isNull()) {
            return saveAsScene();
        }
        if (m_scene->save()) {
            settings.setValue("lastSaved", m_scene->fileName());
            return true;
        }
        else {
            QMessageBox::critical(this, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(m_scene->fileName()).fileName()));
        }
        settings.endGroup();
    }
    return false;
}

bool MainWindow::saveAsScene()
{
    if (m_scene) {
        QSettings settings;
        settings.beginGroup("file");
        QString fileName;
        if (!m_scene->fileName().isNull()) {
            fileName = m_scene->fileName();
        }
        else {
            QFileInfo fileInfo(settings.value("lastSaved", QDir::homePath()).toString());
            fileName = fileInfo.dir().path();
        }
        fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), fileName, fileDialogFilterString);
        if (!fileName.isNull()) {
            if (m_scene->save(fileName)) {
                settings.setValue("lastSaved", fileName);
            }
            else {
                QMessageBox::critical(this, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(fileName).fileName()));
            }
        }
        settings.endGroup();
    }
    return false;
}

bool MainWindow::closeScene(const bool doClose)
{
    if (m_scene) {
        if (m_scene->dirty()) {
            QString fileName = m_scene->fileName().isNull() ? "<i>unnamed</>" : QFileInfo(m_scene->fileName()).fileName();
            QMessageBox::StandardButton button = QMessageBox::question(this, QString(),
                                           QString(tr("The file \"<b>%1</b>\" has unsaved changes.<br/>"
                                                      "Do you want to save it before closing?")).arg(fileName),
                                           QMessageBox::Save | QMessageBox::Discard
                                           | QMessageBox::Cancel,
                                           QMessageBox::Save);
            if (button == QMessageBox::Cancel) {
                return false;
            }
            if (button == QMessageBox::Save) {
                if (!saveScene()) {
                    return false;
                }
            }
            if (doClose) {
                setScene();
            }
        }
        else if (doClose) {
            setScene();
        }
    }
    return true;
}

void MainWindow::setFullscreen(bool fullscreen)
{
    if (fullscreen)
        showFullScreen();
    else
        showNormal();
}

void MainWindow::about()
{
   QFile data(":/text/ABOUT");
   QString text;
   if (data.open(QFile::ReadOnly)) {
       text = QTextStream(&data).readAll();
   }
   QMessageBox::about(this, tr(QString("About %1").arg(QCoreApplication::applicationName()).toLatin1()),
        text.arg(QCoreApplication::applicationName()).toLatin1());
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::license()
{
    QFile data(":/text/LICENSE");
    QString text;
    if (data.open(QFile::ReadOnly)) {
        text = QTextStream(&data).readAll();
    }
    QMessageBox::information(this, QString(), text);
}
