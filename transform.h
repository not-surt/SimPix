#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QObject>
#include <QPointF>
#include <QTransform>

class Transform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(QPointF pan READ pan WRITE setPan NOTIFY panChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(QPointF pixelAspect READ pixelAspect WRITE setPixelAspect NOTIFY pixelAspectChanged)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QTransform matrix READ matrix)
    Q_PROPERTY(QTransform inverseMatrix READ inverseMatrix)
    Q_ENUMS(pan zoom pixelAspect rotation)
public:
    explicit Transform(QObject *parent = nullptr);
    explicit Transform(const Transform &transform, QObject *parent = nullptr);
    QPointF origin() const;
    const QPointF &pan() const;
    qreal zoom() const;
    const QPointF &pixelAspect() const;
    qreal rotation() const;
    const QTransform &matrix();
    const QTransform &inverseMatrix();

signals:
    void originChanged(const QPointF &origin);
    void panChanged(const QPointF &pan);
    void zoomChanged(const qreal zoom);
    void pixelAspectChanged(const QPointF &pixelAspect);
    void rotationChanged(const qreal rotation);
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
