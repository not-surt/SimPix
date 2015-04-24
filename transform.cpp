#include "transform.h"

Transform::Transform(QObject *parent) :
    QObject(parent), m_pan(0.f, 0.f), m_zoom(1.f), m_pixelSize(1.f, 1.f), m_rotation(0.f), dirty(true)
{
}

Transform::Transform(const Transform &transform):
    QObject(transform.parent()), m_pan(transform.m_pan), m_zoom(transform.m_zoom), m_pixelSize(transform.m_pixelSize), m_rotation(transform.m_rotation), dirty(true)
{
}

void Transform::updateMatrices()
{
    m_matrix.setToIdentity();
    m_matrix.rotate(m_rotation, 0.f, 0.f, 1.f);
    m_matrix.scale(m_zoom * m_pixelSize.x(), m_zoom * m_pixelSize.y());
    m_matrix.translate(m_pan.x(), m_pan.y());
    m_inverseMatrix = m_matrix.inverted();
    dirty = false;
}
