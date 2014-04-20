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
    ui(new Ui::MainWindow), image(0)
{
    QCoreApplication::setApplicationName("SimPix");
    QCoreApplication::setOrganizationName("Uninhabitant");
    QCoreApplication::setOrganizationDomain("uninhabitant.com");

    ui->setupUi(this);

    addAction(ui->actionMenu);
    addAction(ui->actionFullscreen);

    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newImage()));
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));
    QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsImage()));
    QObject::connect(ui->actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(setFullscreen(bool)));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));

    QObject::connect(this, SIGNAL(imageChanged(QImage *)), ui->canvas, SLOT(setImage(QImage *)));

    QObject::connect(ui->transformWidget, SIGNAL(transformChanged(Transform)), ui->canvas, SLOT(setTransform(Transform)));
    QObject::connect(ui->canvas, SIGNAL(transformChanged(Transform)), ui->transformWidget, SLOT(setTransform(Transform)));

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

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newImage()
{
    NewDialog *dialog = new NewDialog(this);
    if (dialog->exec()) {
        if (image) {
            delete image;
        }
        image = new QImage(dialog->imageSize(), QImage::Format_RGB32);
        emit imageChanged(image);
    }
}

void MainWindow::openImage()
{
    QSettings settings;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), settings.value("file/lastOpened", QDir::homePath()).toString(), fileDialogFilterString);
    if (!fileName.isNull()) {
        settings.setValue("file/lastOpened", fileName);
        QImage *temp = new QImage(fileName);
        if (temp->isNull()) {
            delete temp;
        }
        else {
            if (image) {
                delete image;
            }
            image = temp;
//            for (int i = 0; i < image->colorTable().length(); i++) {
//                QColor colour = QColor(image->colorTable()[i]);
//                qDebug() << colour;
//            }
//            qDebug() << image->hasAlphaChannel();
            image->save("temp.png");
            emit imageChanged(image);
            ui->paletteWidget->setImage(image);
            setWindowFilePath(fileName);
        }
    }
}

void MainWindow::saveImage()
{
    if (image) {
    }
}

void MainWindow::saveAsImage()
{
    QSettings settings;
    if (image) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), settings.value("file/lastSaved", QDir::homePath()).toString(), fileDialogFilterString);
        settings.setValue("file/lastSaved", fileName);
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
            tr(QString("<b>%1</b> is a simple pixel editor to allow simple people to edit simple pixels in a simple manner.").arg(QCoreApplication::applicationName()).toLatin1()));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}
