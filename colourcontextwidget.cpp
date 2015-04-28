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

void ColourContextWidget::setColourSlot(const Colour colour, const EditingContext::ColourSlot slot)
{
    ColourSwatchWidget *swatch = nullptr;
    switch (slot) {
    default:
    case EditingContext::ColourSlot::Primary:
        swatch = ui->primaryColourSwatch;
        break;
    case EditingContext::ColourSlot::Secondary:
        swatch = ui->secondaryColourSwatch;
        break;
    case EditingContext::ColourSlot::Background:
        swatch = ui->backgroundColourSwatch;
        break;
    }
    if (swatch) {
        swatch->setColour(colour);
    }
}
