#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QObject>
#include <QPointF>
#include <QTransform>

class Transform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF origin READ origin WRITE setOrigin)
    Q_PROPERTY(QPointF pan READ pan WRITE setPan)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom)
    Q_PROPERTY(QPointF pixelAspect READ pixelAspect WRITE setPixelAspect)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QTransform matrix READ matrix)
    Q_PROPERTY(QTransform inverseMatrix READ inverseMatrix)
    Q_ENUMS(origin pan zoom pixelAspect rotation)
public:
    explicit Transform(QObject *parent = nullptr);
    Transform(const Transform &transform);
    QPointF origin() const;
    const QPointF &pan() const;
    qreal zoom() const;
    const QPointF &pixelAspect() const;
    qreal rotation() const;
    const QTransform &matrix();
    const QTransform &inverseMatrix();
    const Transform &operator=(const Transform& other)
    {
        m_origin = other.m_origin;
        m_pan = other.m_pan;
        m_zoom = other.m_zoom;
        m_pixelAspect = other.m_pixelAspect;
        m_rotation = other.m_rotation;
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
    void setOrigin(const QPointF &origin);
    void setPan(const QPointF &pan);
    void setZoom(const qreal zoom);
    void setPixelAspect(const QPointF &pixelAspect);
    void setRotation(const qreal rotation);

private:
    QPointF m_origin;
    QPointF m_pan;
    qreal m_zoom;
    QPointF m_pixelAspect;
    qreal m_rotation;
    QTransform m_matrix;
    QTransform m_inverseMatrix;
    bool dirty;
    void updateMatrix();
};

#endif // TRANSFORM_H
