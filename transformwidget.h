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
    Transform transform() const;

public slots:
    void setTransform(Transform arg);
    void sendTransform();
    void clearPan();
    void clearZoom();
    void clearPixelAspect();
    void clearRotation();

signals:
    void transformChanged(Transform arg);

private:
    Ui::TransformWidget *ui;
};

#endif // TRANSFORMWIDGET_H
