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

Transform TransformWidget::transform() const
{
    return Transform {QPointF(ui->panXSpinBox->value(), ui->panYSpinBox->value()),
                ui->zoomSpinBox->value(),
                QPointF(ui->pixelAspectXSpinBox->value(), ui->pixelAspectYSpinBox->value()),
                ui->rotationSpinBox->value()};
}

void TransformWidget::setTransform(Transform arg)
{
    if (ui->panXSpinBox->value() != arg.pan.x() ||
            ui->panYSpinBox->value() != arg.pan.y() ||
            ui->zoomSpinBox->value() != arg.zoom ||
            ui->pixelAspectXSpinBox->value() != arg.pixelAspect.x() ||
            ui->pixelAspectYSpinBox->value() != arg.pixelAspect.y() ||
            ui->rotationSpinBox->value() != arg.rotation) {
        ui->panXSpinBox->setValue(arg.pan.x());
        ui->panYSpinBox->setValue(arg.pan.y());
        ui->zoomSpinBox->setValue(arg.zoom);
        ui->pixelAspectXSpinBox->setValue(arg.pixelAspect.x());
        ui->pixelAspectYSpinBox->setValue(arg.pixelAspect.y());
        ui->rotationSpinBox->setValue(arg.rotation);
        emit transformChanged(arg);
    }
}

void TransformWidget::sendTransform() {
    emit transformChanged(transform());
}

void TransformWidget::clearPan() {
    ui->panXSpinBox->setValue(0.);
    ui->panYSpinBox->setValue(0.);
    emit transformChanged(transform());
}

void TransformWidget::clearZoom() {
    ui->zoomSpinBox->setValue(1.);
    emit transformChanged(transform());
}

void TransformWidget::clearPixelAspect() {
    ui->pixelAspectXSpinBox->setValue(1.);
    ui->pixelAspectYSpinBox->setValue(1.);
    emit transformChanged(transform());
}

void TransformWidget::clearRotation() {
    ui->rotationSpinBox->setValue(0.);
    emit transformChanged(transform());
}
