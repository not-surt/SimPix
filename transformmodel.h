#ifndef TRANSFORMMODEL_H
#define TRANSFORMMODEL_H

#include "transform.h"

#include <QAbstractTableModel>

class TransformModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_ENUMS(transform)
public:
    explicit TransformModel(QObject *parent = 0);
    const Transform &transform() const;

public slots:
    void setTransform(const Transform &transform);

signals:
    void transformChanged(const Transform &transform);

private:
    Transform m_transform;
};

#endif // TRANSFORMMODEL_H
