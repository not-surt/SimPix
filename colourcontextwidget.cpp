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

void ColourContextWidget::setContextColour(const uint colour, const int context)
{
    ColourSwatch *swatch = nullptr;
    switch (context) {
    default:
    case Image::Primary:
        swatch = ui->primaryColourSwatch;
        break;
    case Image::Secondary:
        swatch = ui->secondaryColourSwatch;
        break;
    case Image::Eraser:
        swatch = ui->eraserColourSwatch;
        break;
    }
    if (swatch) {
        swatch->setColour(QColor(qRed(colour), qGreen(colour), qBlue(colour), qAlpha(colour)));
    }
}
