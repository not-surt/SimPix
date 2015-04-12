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

void ColourContextWidget::setColourSlot(const uint colour, const EditingContext::ColourSlot slot)
{
    ColourSwatch *swatch = nullptr;
    switch (slot) {
    default:
    case EditingContext::Primary:
        swatch = ui->primaryColourSwatch;
        break;
    case EditingContext::Secondary:
        swatch = ui->secondaryColourSwatch;
        break;
    case EditingContext::Background:
        swatch = ui->backgroundColourSwatch;
        break;
    }
    if (swatch) {
        swatch->setColour(QColor(qRed(colour), qGreen(colour), qBlue(colour), qAlpha(colour)));
    }
}
