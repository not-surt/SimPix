#include "transform.h"
#include <QOpenGLFunctions>

Transform::Transform(QObject *parent) :
    QObject(parent), m_origin(0.f, 0.f, 0.f), m_pan(0.f, 0.f, 0.f), m_zoom(1.f), m_pixelAspect(0.f, 0.f, 0.f), m_rotation(0.f), dirty(true)
{
}

Transform::Transform(const Transform &transform):
    QObject(transform.parent()), m_origin(transform.m_origin), m_pan(transform.m_pan), m_zoom(transform.m_zoom), m_pixelAspect(transform.m_pixelAspect), m_rotation(transform.m_rotation), dirty(true)
{
}

void Transform::setOrigin(const QVector3D &origin)
{
    if (m_origin != origin) {
        dirty = true;
        m_origin = origin;
        emit changed(*this);
    }
}

void Transform::setPan(const QVector3D &pan)
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

void Transform::setPixelAspect(const QVector3D &pixelAspect)
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

const QVector3D &Transform::origin() const
{
    return m_origin;
}

const QVector3D &Transform::pan() const
{
    return m_pan;
}

qreal Transform::zoom() const
{
    return m_zoom;
}

const QVector3D &Transform::pixelAspect() const
{
    return m_pixelAspect;
}

qreal Transform::rotation() const
{
    return m_rotation;
}

const QMatrix4x4 &Transform::matrix()
{
    if (dirty) {
        dirty = false;
        updateMatrices();
    }
    return m_matrix;
}

const QMatrix4x4 &Transform::inverseMatrix()
{
    if (dirty) {
        dirty = false;
        updateMatrices();
    }
    return m_inverseMatrix;
}

void Transform::updateMatrices()
{
    m_matrix = QMatrix4x4();
    m_matrix.translate(m_origin.x(), m_origin.y());
    m_matrix.rotate(m_rotation, 0.f, 0.f, 1.f);
    m_matrix.scale(m_zoom * m_pixelAspect.x(), m_zoom * m_pixelAspect.y());
    m_matrix.translate(m_pan.x(), m_pan.y());
    m_inverseMatrix = m_matrix.inverted();
}
