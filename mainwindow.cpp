#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"

#include <QFileDialog>

const QString MainWindow::fileDialogFilterString = tr("PNG Image Files (*.png)");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), image(0)
{
    ui->setupUi(this);

    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newImage()));
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));
    QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsImage()));

    QObject::connect(this, SIGNAL(imageChanged(QImage *)), ui->canvas, SLOT(setImage(QImage *)));
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), 0, fileDialogFilterString);
    if (!fileName.isNull()) {
        QImage *temp = new QImage(fileName);
        if (temp->isNull()) {
            delete temp;
        }
        else {
            if (image) {
                delete image;
            }
            image = temp;
            emit imageChanged(image);
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
    if (image) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), 0, fileDialogFilterString);
    }
}
