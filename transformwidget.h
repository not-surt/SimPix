#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>
#include <QDebug>

#include "ui_transformwidget.h"

#include "transform.h"

namespace Ui {
class TransformWidget;
}

class TransformWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_ENUMS(transform)

public:
    explicit TransformWidget(QWidget *parent = nullptr);
    ~TransformWidget();
    const Transform &transform() const;

public slots:
    void setTransform(const Transform &transform);

signals:
    void transformChanged(const Transform &transform);

private slots:
    void clearPan();
    void clearZoom();
    void clearPixelSize();
    void clearRotation();
    void updateTransform();

private:
    Ui::TransformWidget *ui;
    Transform m_transform;
};

#endif // TRANSFORMWIDGET_H
