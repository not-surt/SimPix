#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QObject>
#include <QMatrix4x4>
#include <QPointF>


class Transform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF pan READ pan WRITE setPan)
    Q_PROPERTY(float zoom READ zoom WRITE setZoom)
    Q_PROPERTY(QPointF pixelSize READ pixelSize WRITE setPixelSize)
    Q_PROPERTY(float rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QMatrix4x4 matrix READ matrix)
    Q_PROPERTY(QMatrix4x4 inverseMatrix READ inverseMatrix)
    Q_ENUMS(origin pan zoom pixelSize rotation)
public:
    explicit Transform(QObject *parent = nullptr);
    Transform(const Transform &transform);
    const QPointF &pan() const { return m_pan; }
    float zoom() const { return m_zoom; }
    const QPointF &pixelSize() const { return m_pixelSize; }
    float rotation() const { return m_rotation; }
    const QMatrix4x4 &matrix() {
        if (dirty) {
            updateMatrices();
        }
        return m_matrix;
    }
    const QMatrix4x4 &inverseMatrix() {
        if (dirty) {
            updateMatrices();
        }
        return m_inverseMatrix;
    }
    const Transform &operator=(const Transform& other) {
        m_pan = other.m_pan;
        m_zoom = other.m_zoom;
        m_pixelSize = other.m_pixelSize;
        m_rotation = other.m_rotation;
        m_matrix = other.m_matrix;
        m_inverseMatrix = other.m_inverseMatrix;
        dirty = other.dirty;
        emit changed(*this);
        return *this;
    }
    friend inline bool operator==(const Transform& lhs, const Transform& rhs) {
        return lhs.m_pan == rhs.m_pan &&
                lhs.m_zoom == rhs.m_zoom &&
                lhs.m_pixelSize == rhs.m_pixelSize &&
                lhs.m_rotation == rhs.m_rotation;
    }
    friend inline bool operator!=(const Transform& lhs, const Transform& rhs) {
        return !(lhs == rhs);
    }

signals:
    void changed(const Transform &transform);    

public slots:
    void setPan(const QPointF &pan) {
        if (m_pan != pan) {
            dirty = true;
            m_pan = pan;
            emit changed(*this);
        }
    }
    void setZoom(const float zoom) {
        if (m_zoom != zoom) {
            dirty = true;
            m_zoom = zoom;
            emit changed(*this);
        }
    }
    void setPixelSize(const QPointF &pixelSize) {
        if (m_pixelSize != pixelSize) {
            dirty = true;
            m_pixelSize = pixelSize;
            emit changed(*this);
        }
    }
    void setRotation(const float rotation) {
        if (m_rotation != rotation) {
            dirty = true;
            m_rotation = rotation;
            emit changed(*this);
        }
    }

private:
    QPointF m_pan;
    float m_zoom;
    QPointF m_pixelSize;
    float m_rotation;
    QMatrix4x4 m_matrix;
    QMatrix4x4 m_inverseMatrix;
    bool dirty;
    void updateMatrices();
};

#endif // TRANSFORM_H
