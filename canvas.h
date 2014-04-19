#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

typedef struct Transform {
    QPointF pan;
    qreal zoom;
    QPointF pixelAspect;
    qreal rotation;
} Transform;

class Canvas : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QImage *image READ image WRITE setImage)
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_ENUMS(image transform)

public:
    explicit Canvas(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

    QImage * image() const
    {
        return m_image;
    }

    Transform transform() const
    {
        return m_transform;
    }

public slots:

    void setImage(QImage * arg)
    {
        this->m_image = arg;
        m_transform.pan = QPointF(0, 0);
        m_transform.zoom = 1.;
        m_transform.pixelAspect = QPointF(1., 1.);
        m_transform.rotation = 0.;
        updateMatrix();
        update();
        emit transformChanged(m_transform);
    }

    void setTransform(Transform arg)
    {
        if (m_transform.pan != arg.pan ||
                m_transform.zoom != arg.zoom ||
                m_transform.pixelAspect != arg.pixelAspect ||
                m_transform.rotation != arg.rotation) {
            m_transform = arg;
            updateMatrix();
            update();
            emit transformChanged(arg);
        }
    }

signals:
    void transformChanged(Transform arg);

private:
    QImage *m_image;
    Transform m_transform;
    QTransform matrix;
    QTransform inverseMatrix;
    QPoint lastMousePos;
    bool panKeyDown;

    void updateMatrix();
};

#endif // CANVAS_H
