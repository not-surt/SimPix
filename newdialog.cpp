#include "newdialog.h"
#include "ui_newdialog.h"

#include <QMenu>
#include <QSettings>
#include <QDebug>

static const QSize presets[] = {
    {50, 50}, {100, 100}, {200, 200},
    QSize(),
    {64, 64}, {128, 128}, {256, 256},
    QSize(),
    {160, 100}, {320, 200}, {640, 400}, {1280, 800},
    QSize(),
    {320, 240}, {640, 480}, {800, 600}, {1024, 760},
    QSize(),
    {320, 180}, {640, 360}, {1280, 720}, {1920, 1080},
    QSize(), QSize()
};

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);
    QMenu *presetMenu = new QMenu();
    for (int i = 0; presets[i].isValid() || presets[i+1].isValid(); ++i) {
        if (!presets[i].isValid()) {
            presetMenu->addSeparator();
        }
        else {
            const QSize &size = presets[i];
            QAction *action = new QAction(presetMenu);
            action->setText(QString("%1x%2").arg(size.width()).arg(size.height()));
            action->setProperty("size", size);
            presetMenu->addAction(action);
            QObject::connect(action, SIGNAL(triggered()), this, SLOT(setPreset()));
        }
    }
    ui->presetButton->setMenu(presetMenu);

    QSettings settings;
    restoreGeometry(settings.value("window/new/geometry").toByteArray());
    ui->widthSpinBox->setValue(settings.value("window/new/lastWidth").toInt());
    ui->heightSpinBox->setValue(settings.value("window/new/lastHeight").toInt());
}

NewDialog::~NewDialog()
{
    delete ui;
}

void NewDialog::setPreset() {
    const QSize &size = sender()->property("size").toSize();
    ui->widthSpinBox->setValue(size.width());
    ui->heightSpinBox->setValue(size.height());
}

void NewDialog::closeEvent(QCloseEvent *event)
{
    // Why no work?! >:(
    qDebug() << "CLOSE!";
    QDialog::closeEvent(event);
}

QSize NewDialog::imageSize() const
{
    return QSize(ui->widthSpinBox->value(), ui->heightSpinBox->value());
}

NewDialog::Mode NewDialog::mode() const
{
    ui->modeComboBox->currentIndex();
}

int NewDialog::palette() const
{
    ui->modeComboBox->currentIndex();
}


void NewDialog::accept()
{
    QSettings settings;
    settings.setValue("window/new/geometry", saveGeometry());
    settings.setValue("window/new/lastWidth", ui->widthSpinBox->value());
    settings.setValue("window/new/lastHeight", ui->heightSpinBox->value());
    QDialog::accept();
}
