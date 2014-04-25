#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QObject>
#include <QPointF>

class Transform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF pan MEMBER m_pan NOTIFY panChanged)
    Q_PROPERTY(qreal zoom MEMBER m_zoom NOTIFY zoomChanged)
    Q_PROPERTY(QPointF pixelAspect MEMBER m_pixelAspect NOTIFY pixelAspectChanged)
    Q_PROPERTY(qreal rotation MEMBER m_rotation NOTIFY rotationChanged)
    Q_ENUMS(pan zoom pixelAspect rotation)
public:
    explicit Transform(QObject *parent = 0);

signals:
    void panChanged(const QPointF &pan);
    void zoomChanged(const qreal zoom);
    void pixelAspectChanged(const QPointF &pixelAspect);
    void rotationChanged(const qreal rotation);
    void changed(const Transform &transform);

public slots:

private:
    QPointF m_pan;
    qreal m_zoom;
    QPointF m_pixelAspect;
    qreal m_rotation;
};

#endif // TRANSFORM_H
