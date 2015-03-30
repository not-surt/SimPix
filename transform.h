#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QOpenGLFunctions>


class Transform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D origin READ origin WRITE setOrigin)
    Q_PROPERTY(QVector3D pan READ pan WRITE setPan)
    Q_PROPERTY(float zoom READ zoom WRITE setZoom)
    Q_PROPERTY(QVector3D pixelAspect READ pixelAspect WRITE setPixelAspect)
    Q_PROPERTY(float rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QMatrix4x4 matrix READ matrix)
    Q_PROPERTY(QMatrix4x4 inverseMatrix READ inverseMatrix)
    Q_ENUMS(origin pan zoom pixelAspect rotation)
public:
    explicit Transform(QObject *parent = nullptr);
    Transform(const Transform &transform);
    const QVector3D &origin() const;
    const QVector3D &pan() const;
    float zoom() const;
    const QVector3D &pixelAspect() const;
    float rotation() const;
    const QMatrix4x4 &matrix();
    const QMatrix4x4 &inverseMatrix();
    const Transform &operator=(const Transform& other)
    {
        m_origin = other.m_origin;
        m_pan = other.m_pan;
        m_zoom = other.m_zoom;
        m_pixelAspect = other.m_pixelAspect;
        m_rotation = other.m_rotation;
        m_matrix = other.m_matrix;
        m_inverseMatrix = other.m_inverseMatrix;
        matricesDirty = other.matricesDirty;
        emit changed(*this);
        return *this;
    }
    friend inline bool operator==(const Transform& lhs, const Transform& rhs)
    {
        return lhs.m_origin == rhs.m_origin &&
                lhs.m_pan == rhs.m_pan &&
                lhs.m_zoom == rhs.m_zoom &&
                lhs.m_pixelAspect == rhs.m_pixelAspect &&
                lhs.m_rotation == rhs.m_rotation;
    }
    friend inline bool operator!=(const Transform& lhs, const Transform& rhs)
    {
        return !(lhs == rhs);
    }

signals:
    void changed(const Transform &transform);    

public slots:
    void setOrigin(const QVector3D &origin);
    void setPan(const QVector3D &pan);
    void setZoom(const float zoom);
    void setPixelAspect(const QVector3D &pixelAspect);
    void setRotation(const float rotation);

private:
    QVector3D m_origin;
    QVector3D m_pan;
    float m_zoom;
    QVector3D m_pixelAspect;
    float m_rotation;
    QMatrix4x4 m_matrix;
    QMatrix4x4 m_inverseMatrix;
    bool matricesDirty;
    void updateMatrices();
};

#endif // TRANSFORM_H
