#include "transformmodel.h"

TransformModel::TransformModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

const Transform &TransformModel::transform() const
{
    return m_transform;
}

void TransformModel::setTransform(const Transform &transform)
{
    if (m_transform != transform) {
        m_transform = transform;
        emit transformChanged(m_transform);
    }
}
