#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "util.h"
#include "colourswatch.h"
#include "statusmousewidget.h"
#include "scenewindow.h"
#include "application.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>

const QString MainWindow::fileDialogFilterString = tr("PNG Image Files (*.png)");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_scene(0)
{    
    canvasBackgroundPixmap = generateBackgroundPixmap(32);
    swatchBackgroundPixmap = generateBackgroundPixmap(16);

    ui->setupUi(this);

    m_sceneWindow = new SceneWindow(APP->context());
    setCentralWidget(QWidget::createWindowContainer(m_sceneWindow));

    // Copy actions to window. Is there a better way?
    QList<QMenu *> menus = ui->menuBar->findChildren<QMenu *>();
    QListIterator<QMenu *> menu(menus);
    while (menu.hasNext()) {
        addActions(menu.next()->findChildren<QAction *>());
    }

    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newImage()));
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));
    QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsImage()));
    QObject::connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeImage()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionMenu, SIGNAL(triggered(bool)), this->menuBar(), SLOT(setVisible(bool)));
    QObject::connect(ui->actionStatusBar, SIGNAL(triggered(bool)), this->statusBar(), SLOT(setVisible(bool)));
    QObject::connect(ui->actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(setFullscreen(bool)));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    QObject::connect(ui->actionLicense, SIGNAL(triggered()), this, SLOT(license()));

    QObject::connect(this, SIGNAL(imageChanged(Scene *const)), m_sceneWindow, SLOT(setImage(Scene *const)));
    QObject::connect(this, SIGNAL(imageChanged(Scene *const)), ui->paletteWidget, SLOT(setImage(Scene *const)));
    QObject::connect(ui->paletteWidget, SIGNAL(colourChanged(const uint)), m_sceneWindow, SLOT(updateImage()));

    QObject::connect(ui->transformWidget, SIGNAL(transformChanged(Transform)), m_sceneWindow, SLOT(setTransform(Transform)));
    QObject::connect(m_sceneWindow, SIGNAL(transformChanged(Transform)), ui->transformWidget, SLOT(setTransform(Transform)));

    QObject::connect(ui->actionTiled, SIGNAL(triggered(bool)), m_sceneWindow, SLOT(setTiled(bool)));
    QObject::connect(ui->actionShowFrame, SIGNAL(triggered(bool)), m_sceneWindow, SLOT(setShowFrame(bool)));
    QObject::connect(ui->actionAlpha, SIGNAL(triggered(bool)), m_sceneWindow, SLOT(setShowAlpha(bool)));

    QMenu *toolBarMenu = new QMenu(this);
    ui->actionToolbars->setMenu(toolBarMenu);
    QList<QToolBar *> toolbars = findChildren<QToolBar *>();
    QListIterator<QToolBar *> toolbar(toolbars);
    while (toolbar.hasNext()) {
        toolBarMenu->addAction(toolbar.next()->toggleViewAction());
    }

    QMenu *dockMenu = new QMenu(this);
    ui->actionDocks->setMenu(dockMenu);
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    QListIterator<QDockWidget *> dock(docks);
    while (dock.hasNext()) {
        dockMenu->addAction(dock.next()->toggleViewAction());
    }

    StatusMouseWidget *statusMouseWidget = new StatusMouseWidget();
//    statusMouseWidget->hide();
    statusBar()->addWidget(statusMouseWidget);
//    QObject::connect(canvas, SIGNAL(mouseEntered()), statusMouseWidget, SLOT(show()));
//    QObject::connect(canvas, SIGNAL(mouseLeft()), statusMouseWidget, SLOT(hide()));
    QObject::connect(m_sceneWindow, SIGNAL(mousePixelChanged(QPoint, uint, int)), statusMouseWidget, SLOT(setMouseInfo(QPoint, uint, int)));

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

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_scene;
    delete canvasBackgroundPixmap;
    delete swatchBackgroundPixmap;
}

Scene *MainWindow::image() const
{
    return m_scene;
}

void MainWindow::setImage(Scene *image)
{
    if (m_scene != image) {
        if (image) {
            QObject::connect(image, SIGNAL(changed(const QRegion &)), m_sceneWindow, SLOT(updateImage(const QRegion &)));
            QObject::connect(image, SIGNAL(contextColourChanged(const uint, const int)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::connect(m_sceneWindow, SIGNAL(clicked(const QPoint &)), image, SLOT(point(const QPoint &)));
            QObject::connect(m_sceneWindow, SIGNAL(dragged(const QPoint &, const QPoint &)), image, SLOT(stroke(const QPoint &, const QPoint &)));
            QObject::connect(ui->actionUndo, SIGNAL(triggered()), image->undoStack, SLOT(undo()));
            QObject::connect(ui->actionRedo, SIGNAL(triggered()), image->undoStack, SLOT(redo()));
            setWindowFilePath(image->fileName());
        }
        else {
            setWindowFilePath(QString());
        }
        emit imageChanged(image);
        if (m_scene) {
            QObject::disconnect(m_scene, SIGNAL(changed(const QRegion &)), m_sceneWindow, SLOT(updateImage(const QRegion &)));
            QObject::disconnect(m_scene, SIGNAL(contextColourChanged(const uint, const int)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::disconnect(m_sceneWindow, SIGNAL(clicked(const QPoint &)), m_scene, SLOT(point(const QPoint &)));
            QObject::disconnect(m_sceneWindow, SIGNAL(dragged(const QPoint &, const QPoint &)), m_scene, SLOT(stroke(const QPoint &, const QPoint &)));
            QObject::disconnect(ui->actionUndo, SIGNAL(triggered()), m_scene->undoStack, SLOT(undo()));
            QObject::disconnect(ui->actionRedo, SIGNAL(triggered()), m_scene->undoStack, SLOT(redo()));
            m_scene->deleteLater();
        }
        m_scene = image;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!closeImage()) {
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

bool MainWindow::newImage()
{
    if (!closeImage(false)) {
        return false;
    }
    NewDialog *dialog = new NewDialog(this);
    if (dialog->exec()) {
        Scene *newImage = new Scene(dialog->imageSize(), dialog->mode());
        setImage(newImage);
        return true;
    }
    return false;
}

bool MainWindow::openImage()
{
    if (!closeImage(false)) {
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
            setImage(newImage);
            return true;
        }
    }
    settings.endGroup();
    return false;
}

bool MainWindow::saveImage()
{
    if (m_scene) {
        QSettings settings;
        settings.beginGroup("file");
        if (m_scene->fileName().isNull()) {
            return saveAsImage();
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

bool MainWindow::saveAsImage()
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

bool MainWindow::closeImage(const bool doClose)
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
                if (!saveImage()) {
                    return false;
                }
            }
            if (doClose) {
                setImage();
            }
        }
        else if (doClose) {
            setImage();
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
   QMessageBox::about(this, tr(QString("About %1").arg(QCoreApplication::applicationName()).toLatin1()),
        tr(QString(
            "<p><b>%1</b> is a simple pixel editor to allow simple people to edit simple pixels in a simple manner.</p>"
            "<p>More about %1:"
            "<ul>"
            "<li><a href=\"https://github.com/not-surt/SimPix\">GitHub page</a></li>"
            "</ul></p>"
            "<p>%1 is developed by:"
            "<ul>"
            "<li><b>surt</b> aka Carl Olsson - <a href=\"http://uninhabitant.com\">personal site</a></li>"
            "</ul></p>"
            "<p>%1 makes use of the following projects:"
            "<ul>"
            "<li><b>Qt</b> - <a href=\"http://qt-project.org\">project site</a></li>"
            "</ul></p>"
            ).arg(QCoreApplication::applicationName()).toLatin1()));
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
        QTextStream stream(&data);
        QString line;
        do {
            line = stream.readLine();
            text += line + '\n';
        } while (!line.isNull());
    }
    QMessageBox::information(this, QString(), text);
}
