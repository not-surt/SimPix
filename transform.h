#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QObject>
#include <QMatrix4x4>
#include <QVector3D>


class Transform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D pan READ pan WRITE setPan)
    Q_PROPERTY(float zoom READ zoom WRITE setZoom)
    Q_PROPERTY(QVector3D pixelSize READ pixelSize WRITE setPixelSize)
    Q_PROPERTY(float rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QMatrix4x4 worldToClip READ worldToClip)
    Q_PROPERTY(QMatrix4x4 clipToWorld READ clipToWorld)
    Q_ENUMS(origin pan zoom pixelSize rotation)
public:
    explicit Transform(QObject *parent = nullptr);
    Transform(const Transform &transform);
    const QVector3D &pan() const { return m_pan; }
    float zoom() const { return m_zoom; }
    const QVector3D &pixelSize() const { return m_pixelSize; }
    float rotation() const { return m_rotation; }
    const QMatrix4x4 &worldToClip() {
        if (dirty) {
            updateMatrices();
        }
        return m_worldToClip;
    }
    const QMatrix4x4 &clipToWorld() {
        if (dirty) {
            updateMatrices();
        }
        return m_clipToWorld;
    }
    const Transform &operator=(const Transform& other) {
        m_pan = other.m_pan;
        m_zoom = other.m_zoom;
        m_pixelSize = other.m_pixelSize;
        m_rotation = other.m_rotation;
        m_worldToClip = other.m_worldToClip;
        m_clipToWorld = other.m_clipToWorld;
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
    void setPan(const QVector3D &pan) {
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
    void setPixelSize(const QVector3D &pixelSize) {
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
    QVector3D m_pan;
    float m_zoom;
    QVector3D m_pixelSize;
    float m_rotation;
    QMatrix4x4 m_worldToClip;
    QMatrix4x4 m_clipToWorld;
    bool dirty;
    void updateMatrices();
};

#endif // TRANSFORM_H
