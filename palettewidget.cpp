#include "palettemodel.h"
#include "palettewidget.h"
#include "ui_palettewidget.h"

#include <QColorDialog>
#include <QItemDelegate>
#include <QPainter>
#include <QStringListModel>

#include "color_dialog.hpp"

PaletteWidget::PaletteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaletteWidget)
{
    ui->setupUi(this);

    QObject::connect(ui->paletteView, SIGNAL(clicked(QModelIndex)), this, SLOT(setPrimaryColour(QModelIndex)));
//    QObject::connect(ui->paletteView, SIGNAL(rightClicked(QModelIndex)), this, SLOT(setSecondaryColour(QModelIndex)));
    QObject::connect(ui->paletteView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editColour(QModelIndex)));
}

PaletteWidget::~PaletteWidget()
{
    delete ui;
}

Image *PaletteWidget::image() const
{
    return ui->paletteView->image();
}

void PaletteWidget::setImage(Image *image)
{
    ui->paletteView->setImage(image);
}

void PaletteWidget::editColour(QModelIndex index)
{
    if (index.isValid()) {
        const QColor colour = index.data().value<QColor>();
        qDebug() << colour;
//        const QColor result = QColorDialog::getColor(colour, this, tr("Edit Colour"), QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
//        const QColor result = QColorDialog::getColor(colour, this);
        Color_Dialog dialog(this);
        dialog.setColor(colour);
        dialog.exec();
    }
}

void PaletteWidget::setPrimaryColour(QModelIndex index)
{
    qDebug() << index.row();
}

void PaletteWidget::setSecondaryColour(QModelIndex index)
{
    qDebug() << index.row();
}


