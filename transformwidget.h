#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>
#include <QDebug>

#include "ui_transformwidget.h"

#include "canvas.h"
#include "constants.h"

namespace Ui {
class TransformWidget;
}

class TransformWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_ENUMS(transform)

public:
    explicit TransformWidget(QWidget *parent = 0);
    ~TransformWidget();
    Transform transform() const
    {
        return Transform {QPointF(ui->panXSpinBox->value(), ui->panYSpinBox->value()),
                         ui->zoomSpinBox->value(),
                         QPointF(ui->pixelAspectXSpinBox->value(), ui->pixelAspectYSpinBox->value()),
                         ui->rotationSpinBox->value()};
    }

public slots:
    void setTransform(Transform arg)
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
    void sendTransform() {
        emit transformChanged(transform());
    }
    void clearPan() {
        ui->panXSpinBox->setValue(PAN_DEF);
        ui->panYSpinBox->setValue(PAN_DEF);
        emit transformChanged(transform());
    }
    void clearZoom() {
        ui->zoomSpinBox->setValue(ZOOM_DEF);
        emit transformChanged(transform());
    }
    void clearPixelAspect() {
        ui->pixelAspectXSpinBox->setValue(PIXEL_ASPECT_DEF);
        ui->pixelAspectYSpinBox->setValue(PIXEL_ASPECT_DEF);
        emit transformChanged(transform());
    }
    void clearRotation() {
        ui->rotationSpinBox->setValue(ROTATION_DEF);
        emit transformChanged(transform());
    }

signals:
    void transformChanged(Transform arg);

private:
    Ui::TransformWidget *ui;
};

#endif // TRANSFORMWIDGET_H
