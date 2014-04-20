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

    QImage * image() const;

    Transform transform() const;

public slots:

    void setImage(QImage * arg);

    void setTransform(Transform arg);

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
