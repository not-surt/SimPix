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
    case ImageDocument::Primary:
        swatch = ui->primaryColourSwatch;
        break;
    case ImageDocument::Secondary:
        swatch = ui->secondaryColourSwatch;
        break;
    case ImageDocument::Eraser:
        swatch = ui->eraserColourSwatch;
        break;
    }
    if (swatch) {
        swatch->setColour(QColor(qRed(colour), qGreen(colour), qBlue(colour), qAlpha(colour)));
    }
}
