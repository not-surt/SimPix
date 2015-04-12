#include "colourcontextwidget.h"
#include "ui_colourcontextwidget.h"

ColourContextWidget::ColourContextWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColourContextWidget)
{
    ui->setupUi(this);
}

ColourContextWidget::~ColourContextWidget()
{
    delete ui;
}

void ColourContextWidget::setColourSlot(const EditingContext::ColourSlot colour, const EditingContext::ColourSlotId slot)
{
    ColourSwatch *swatch = nullptr;
    switch (slot) {
    default:
    case EditingContext::ColourSlotId::Primary:
        swatch = ui->primaryColourSwatch;
        break;
    case EditingContext::ColourSlotId::Secondary:
        swatch = ui->secondaryColourSwatch;
        break;
    case EditingContext::ColourSlotId::Background:
        swatch = ui->backgroundColourSwatch;
        break;
    }
    if (swatch) {
        swatch->setColour(QColor(qRed(colour.rgba), qGreen(colour.rgba), qBlue(colour.rgba), qAlpha(colour.rgba)));
    }
}
