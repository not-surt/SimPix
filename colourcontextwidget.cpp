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
