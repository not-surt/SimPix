#include "newdialog.h"

#include <QMenu>
#include <QSettings>
#include <QDebug>
#include "application.h"

static const QSize sizePresets[] = {
    {50, 50}, {100, 100}, {200, 200},
    QSize(),
    {64, 64}, {128, 128}, {256, 256},
    QSize(),
    {160, 100}, {320, 200}, {640, 400}, {1280, 800},
    QSize(),
    {320, 240}, {640, 480}, {800, 600}, {1024, 768},
    QSize(),
    {320, 180}, {640, 360}, {1280, 720}, {1920, 1080},
    QSize(), QSize()
};

static const QSizeF pixelSizePresets[] = {
    {1., 1.},
    {2., 1.},
    {1., 2.},
    {1., 1.2},
    QSize(), QSize()
};

typedef struct ImagePreset {
    QSize size;
    QSize pixelSize;
    QImage::Format format;
    QString palette;
    QVariant fill;
    QVariant transparent;
} ImagePreset;

static const QSizeF imagePresets[] = {
    {1., 1.},
    {2., 1.},
    {1., 2.},
    {1., 1.2},
    QSize(), QSize()
};

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);

//    ui->formatComboBox->addItem("Indexed", (int)TextureData::Format::Indexed);
//    ui->formatComboBox->addItem("RGBA", (int)TextureData::Format::RGBA);
    static const int LENGTH = sizeof(TextureData::FORMATS) / sizeof(TextureData::Format);
    for (int i = 0; i < LENGTH; i++) {
        ui->formatComboBox->addItem(TextureData::FORMATS[i].name, (int)TextureData::FORMATS[i].id);
    }

    QMenu *presetMenu = new QMenu;
    for (int i = 0; sizePresets[i].isValid() || sizePresets[i+1].isValid(); ++i) {
        if (!sizePresets[i].isValid()) {
            presetMenu->addSeparator();
        }
        else {
            const QSize &size = sizePresets[i];
            QAction *action = new QAction(presetMenu);
            action->setText(QString("%1x%2").arg(size.width()).arg(size.height()));
            action->setProperty("size", size);
            presetMenu->addAction(action);
            QObject::connect(action, &QAction::triggered, this, &NewDialog::setPreset);
        }
    }
    ui->presetButton->setMenu(presetMenu);

    restoreGeometry(APP->settings.value("window/new/geometry").toByteArray());
    ui->widthSpinBox->setValue(APP->settings.value("window/new/lastSize").toSize().width());
    ui->heightSpinBox->setValue(APP->settings.value("window/new/lastSize").toSize().height());
    ui->formatComboBox->setCurrentIndex(APP->settings.value("window/new/lastFormat").toInt());
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
    QDialog::closeEvent(event);
}

void NewDialog::accept()
{
    APP->settings.setValue("window/new/geometry", saveGeometry());
    APP->settings.setValue("window/new/lastSize", QSize(ui->widthSpinBox->value(), ui->heightSpinBox->value()));
    APP->settings.setValue("window/new/lastFormat", ui->formatComboBox->currentIndex());
    QDialog::accept();
}
