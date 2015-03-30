#include "transformwidget.h"
#include "ui_transformwidget.h"

#include "util.h"

TransformWidget::TransformWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformWidget)
{
    ui->setupUi(this);
    QObject::connect(ui->panXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTransform()));
    QObject::connect(ui->panYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTransform()));
    QObject::connect(ui->zoomSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTransform()));
    QObject::connect(ui->pixelAspectXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTransform()));
    QObject::connect(ui->pixelAspectYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTransform()));
    QObject::connect(ui->rotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTransform()));

    QObject::connect(ui->panClearButton, SIGNAL(clicked()), this, SLOT(clearPan()));
    QObject::connect(ui->zoomClearButton, SIGNAL(clicked()), this, SLOT(clearZoom()));
    QObject::connect(ui->pixelAspectClearButton, SIGNAL(clicked()), this, SLOT(clearPixelAspect()));
    QObject::connect(ui->rotationClearButton, SIGNAL(clicked()), this, SLOT(clearRotation()));
}

TransformWidget::~TransformWidget()
{
    delete ui;
}

const Transform &TransformWidget::transform() const
{
    return m_transform;
}


void TransformWidget::setTransform(const Transform &transform)
{
    if (m_transform != transform) {
        m_transform = transform;
        ObjectSignalBlocker blocker(this);
        ui->panXSpinBox->setValue(m_transform.pan().x());
        ui->panYSpinBox->setValue(m_transform.pan().y());
        ui->zoomSpinBox->setValue(m_transform.zoom());
        ui->pixelAspectXSpinBox->setValue(m_transform.pixelAspect().x());
        ui->pixelAspectYSpinBox->setValue(m_transform.pixelAspect().y());
        ui->rotationSpinBox->setValue(m_transform.rotation());
        emit transformChanged(m_transform);
    }
}

void TransformWidget::updateTransform() {
    Transform transform;
    transform.setOrigin(m_transform.origin());
    transform.setPan(QVector3D(ui->panXSpinBox->value(), ui->panYSpinBox->value(), 0.f));
    transform.setZoom(ui->zoomSpinBox->value());
    transform.setPixelAspect(QVector3D(ui->pixelAspectXSpinBox->value(), ui->pixelAspectYSpinBox->value(), 0.f));
    transform.setRotation(ui->rotationSpinBox->value());
    if (m_transform != transform) {
        emit transformChanged(transform);
    }
}

void TransformWidget::clearPan() {
    ui->panXSpinBox->setValue(0.);
    ui->panYSpinBox->setValue(0.);
    updateTransform();
}

void TransformWidget::clearZoom() {
    ui->zoomSpinBox->setValue(1.);
    updateTransform();
}

void TransformWidget::clearPixelAspect() {
    ui->pixelAspectXSpinBox->setValue(1.);
    ui->pixelAspectYSpinBox->setValue(1.);
    updateTransform();
}

void TransformWidget::clearRotation() {
    ui->rotationSpinBox->setValue(0.);
    updateTransform();
}
