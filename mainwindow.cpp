#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "canvas.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

const QString MainWindow::fileDialogFilterString = tr("PNG Image Files (*.png)");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_image(0)
{    
//    QAction *undoAction = undoStack->createUndoAction(this, tr("&Undo"));
//    undoAction->setShortcuts(QKeySequence::Undo);
//    QAction *redoAction = undoStack->createRedoAction(this, tr("&Redo"));
//    redoAction->setShortcuts(QKeySequence::Redo);

    ui->setupUi(this);

    addAction(ui->actionMenu);
    addAction(ui->actionFullscreen);

    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newImage()));
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));
    QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsImage()));
    QObject::connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeFile()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(setFullscreen(bool)));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    QObject::connect(ui->actionLicense, SIGNAL(triggered()), this, SLOT(license()));

    QObject::connect(this, SIGNAL(imageChanged(Image *const)), ui->canvas, SLOT(setImage(Image *const)));
    QObject::connect(this, SIGNAL(imageChanged(Image *const)), ui->paletteWidget, SLOT(setImage(Image *const)));

    QObject::connect(ui->transformWidget, SIGNAL(transformChanged(Transform)), ui->canvas, SLOT(setTransform(Transform)));
    QObject::connect(ui->canvas, SIGNAL(transformChanged(Transform)), ui->transformWidget, SLOT(setTransform(Transform)));

    QObject::connect(ui->actionTiled, SIGNAL(triggered(bool)), ui->canvas, SLOT(setTiled(bool)));

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
    delete m_image;
}

Image *MainWindow::image() const
{
    return m_image;
}

void MainWindow::setImage(Image *image)
{
    if (m_image != image) {
        QObject::connect(image, SIGNAL(changed(const QRegion &)), ui->canvas, SLOT(updateImage(const QRegion &)));
        QObject::connect(ui->canvas, SIGNAL(clicked(const QPoint &)), image, SLOT(point(const QPoint &)));
        QObject::connect(ui->canvas, SIGNAL(dragged(const QPoint &, const QPoint &)), image, SLOT(stroke(const QPoint &, const QPoint &)));
        emit imageChanged(image);
        if (m_image) {
            m_image->deleteLater();
        }
        m_image = image;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup("window");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.endGroup();
    QMainWindow::closeEvent(event);
}

void MainWindow::newImage()
{
    NewDialog *dialog = new NewDialog(this);
    if (dialog->exec()) {
        Image *newImage = new Image(dialog->imageSize(), dialog->mode());
        setImage(newImage);
    }
}

void MainWindow::openImage()
{
    QSettings settings;
    settings.beginGroup("file");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), settings.value("lastOpened", QDir::homePath()).toString(), fileDialogFilterString);
    if (!fileName.isNull()) {
        settings.setValue("lastOpened", fileName);
        Image *newImage = new Image(fileName);
        if (newImage->data().isNull()) {
            delete newImage;
        }
        else {
//            newImage->save("temp.png");
            setImage(newImage);
        }
    }
    settings.endGroup();
}

void MainWindow::saveImage()
{
    if (m_image) {
    }
}

void MainWindow::saveAsImage()
{
    if (m_image) {
        QSettings settings;
        settings.beginGroup("file");
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), settings.value("lastSaved", QDir::homePath()).toString(), fileDialogFilterString);
        settings.setValue("lastSaved", fileName);
        settings.endGroup();
    }
}

void MainWindow::closeFile()
{
    if (m_image) {
        Image *temp = m_image;
        m_image = 0;
        ui->paletteWidget->setImage(m_image);
        emit imageChanged(m_image);
        delete temp;
    }
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
                   "<li>Mattia Basaglia's <b>Qt-Color-Picker</b> - <a href=\"https://github.com/mbasaglia/Qt-Color-Picker\">GitHub page</a></li>"
                   "</ul></p>"
                   ).arg(QCoreApplication::applicationName()).toLatin1()));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::license()
{
    QFile data("LICENSE");
    QString text;
    if (data.open(QFile::ReadOnly)) {
        QTextStream stream(&data);
        QString line;
        do {
            line = stream.readLine();
            text += line + '\n';
        } while (!line.isNull());
    }
    QMessageBox::information(this, tr(QString("%1 License").arg(QCoreApplication::applicationName()).toLatin1()), text);
}
