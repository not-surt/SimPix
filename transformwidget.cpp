#include "transformwidget.h"
#include "ui_transformwidget.h"

TransformWidget::TransformWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformWidget)
{
    ui->setupUi(this);
    QObject::connect(ui->panXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(sendTransform()));
    QObject::connect(ui->panYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(sendTransform()));
    QObject::connect(ui->zoomSpinBox, SIGNAL(valueChanged(double)), this, SLOT(sendTransform()));
    QObject::connect(ui->pixelAspectXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(sendTransform()));
    QObject::connect(ui->pixelAspectYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(sendTransform()));
    QObject::connect(ui->rotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(sendTransform()));

    QObject::connect(ui->panClearButton, SIGNAL(clicked()), this, SLOT(clearPan()));
    QObject::connect(ui->zoomClearButton, SIGNAL(clicked()), this, SLOT(clearZoom()));
    QObject::connect(ui->pixelAspectClearButton, SIGNAL(clicked()), this, SLOT(clearPixelAspect()));
    QObject::connect(ui->rotationClearButton, SIGNAL(clicked()), this, SLOT(clearRotation()));
}

TransformWidget::~TransformWidget()
{
    delete ui;
}
