#include "palettemodel.h"
#include "palettewidget.h"
#include "ui_palettewidget.h"

#include <QColorDialog>
#include <QItemDelegate>
#include <QPainter>
#include <QStringListModel>

PaletteWidget::PaletteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaletteWidget)
{
    ui->setupUi(this);
}

PaletteWidget::~PaletteWidget()
{
    delete ui;
}

EditingContext *PaletteWidget::editingContext() const
{
    return ui->paletteView->editingContext();
}

void PaletteWidget::setEditingContext(EditingContext *const editingContext)
{
    if (ui->paletteView->editingContext()) {
        QObject::disconnect(ui->paletteView, SIGNAL(clicked(QModelIndex)), this, SLOT(setColour(QModelIndex)));
        QObject::disconnect(ui->paletteView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editColour(QModelIndex)));
    }
    ui->paletteView->setEditingContext(editingContext);
    QObject::connect(ui->paletteView, SIGNAL(clicked(QModelIndex)), this, SLOT(setColour(QModelIndex)));
    QObject::connect(ui->paletteView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editColour(QModelIndex)));
}

void PaletteWidget::editColour(QModelIndex index)
{
    if (index.isValid()) {
        const QColor colour = index.data().value<QColor>();
//        const QColor result = QColorDialog::getColor(colour, this, tr("Edit Colour"), QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
        const QColor result = QColorDialog::getColor(colour, this, QString(), QColorDialog::ShowAlphaChannel);
        if (result.isValid()) {
            ui->paletteView->model()->setData(index, result);
            emit colourChanged(index.row());
        }
    }
}

void PaletteWidget::setColour(QModelIndex index)
{
    if (ui->paletteView->editingContext()) {
//        const QColor colour = index.data().value<QColor>();
//        ui->paletteView->image()->setContextColour(colour.rgba(), ui->paletteView->image()->activeContextColour());
        ui->paletteView->editingContext()->setColourSlot(index.row(), ui->paletteView->editingContext()->activeColourSlot());
    }
}


