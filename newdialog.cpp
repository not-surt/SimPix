#include "newdialog.h"
#include "ui_newdialog.h"

static const QSize presets[] = {
    {50, 50}, {100, 100}, {200, 200},
    QSize()
};

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);
}

NewDialog::~NewDialog()
{
    delete ui;
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
