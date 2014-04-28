#include "transform.h"

Transform::Transform(QObject *parent) :
    QObject(parent), m_origin(0., 0.), m_pan(0., 0.), m_zoom(1.), m_pixelAspect(1., 1.), m_rotation(0.), dirty(true)
{
}

Transform::Transform(const Transform &transform):
    QObject(transform.parent()), m_origin(transform.m_origin), m_pan(transform.m_pan), m_zoom(transform.m_zoom), m_pixelAspect(transform.m_pixelAspect), m_rotation(transform.m_rotation), dirty(true)
{
}

void Transform::setOrigin(const QPointF &origin)
{
    if (m_origin != origin) {
        dirty = true;
        m_origin = origin;
        emit changed(*this);
    }
}

void Transform::setPan(const QPointF &pan)
{
    if (m_pan != pan) {
        dirty = true;
        m_pan = pan;
        emit changed(*this);
    }
}

void Transform::setZoom(const qreal zoom)
{
    if (m_zoom != zoom) {
        dirty = true;
        m_zoom = zoom;
        emit changed(*this);
    }
}

void Transform::setPixelAspect(const QPointF &pixelAspect)
{
    if (m_pixelAspect != pixelAspect) {
        dirty = true;
        m_pixelAspect = pixelAspect;
        emit changed(*this);
    }
}

void Transform::setRotation(const qreal rotation)
{
    if (m_rotation != rotation) {
        dirty = true;
        m_rotation = rotation;
        emit changed(*this);
    }
}

const QPointF &Transform::pan() const
{
    return m_pan;
}

qreal Transform::zoom() const
{
    return m_zoom;
}

const QPointF &Transform::pixelAspect() const
{
    return m_pixelAspect;
}

qreal Transform::rotation() const
{
    return m_rotation;
}

const QTransform &Transform::matrix()
{
    if (dirty) {
        dirty = false;
        updateMatrix();
    }
    return m_matrix;
}

const QTransform &Transform::inverseMatrix()
{
    if (dirty) {
        dirty = false;
        updateMatrix();
    }
    return m_inverseMatrix;
}

QPointF Transform::origin() const
{
    return m_origin;
}

void Transform::updateMatrix()
{
    m_matrix = QTransform();
    m_matrix.translate(m_origin.x(), m_origin.y());
    m_matrix.rotate(m_rotation);
    m_matrix.scale(m_zoom * m_pixelAspect.x(), m_zoom * m_pixelAspect.y());
    m_matrix.translate(m_pan.x(), m_pan.y());
    m_inverseMatrix = m_matrix.inverted();
}
