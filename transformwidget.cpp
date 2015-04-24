#include "transformwidget.h"
#include "ui_transformwidget.h"

#include "util.h"

TransformWidget::TransformWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformWidget)
{
    ui->setupUi(this);
    auto func = [this](double d) { updateTransform(); };
    auto signal = SS_CAST(QDoubleSpinBox, valueChanged, double);
    QObject::connect(ui->panXSpinBox, signal, func);
    QObject::connect(ui->panYSpinBox, signal, func);
    QObject::connect(ui->zoomSpinBox, signal, func);
    QObject::connect(ui->pixelSizeXSpinBox, signal, func);
    QObject::connect(ui->pixelSizeYSpinBox, signal, func);
    QObject::connect(ui->rotationSpinBox, signal, func);

    QObject::connect(ui->panClearButton, &QToolButton::clicked, this, &TransformWidget::clearPan);
    QObject::connect(ui->zoomClearButton, &QToolButton::clicked, this, &TransformWidget::clearZoom);
    QObject::connect(ui->pixelSizeClearButton, &QToolButton::clicked, this, &TransformWidget::clearPixelSize);
    QObject::connect(ui->rotationClearButton, &QToolButton::clicked, this, &TransformWidget::clearRotation);
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
        {
            ObjectSignalBlocker blocker(this);
            ui->panXSpinBox->setValue(m_transform.pan().x());
            ui->panYSpinBox->setValue(m_transform.pan().y());
            ui->zoomSpinBox->setValue(m_transform.zoom());
            ui->pixelSizeXSpinBox->setValue(m_transform.pixelSize().x());
            ui->pixelSizeYSpinBox->setValue(m_transform.pixelSize().y());
            ui->rotationSpinBox->setValue(m_transform.rotation());
        }
        emit transformChanged(m_transform);
    }
}

void TransformWidget::updateTransform() {
    Transform transform;
    transform.setPan(QPointF(ui->panXSpinBox->value(), ui->panYSpinBox->value()));
    transform.setZoom(ui->zoomSpinBox->value());
    transform.setPixelSize(QPointF(ui->pixelSizeXSpinBox->value(), ui->pixelSizeYSpinBox->value()));
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

void TransformWidget::clearPixelSize() {
    ui->pixelSizeXSpinBox->setValue(1.);
    ui->pixelSizeYSpinBox->setValue(1.);
    updateTransform();
}

void TransformWidget::clearRotation() {
    ui->rotationSpinBox->setValue(0.);
    updateTransform();
}
