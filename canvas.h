#ifndef CANVAS_H
#define CANVAS_H

#include <QUndoCommand>
#include <QWidget>
#include "image.h"

typedef struct Transform {
    QPointF pan;
    qreal zoom;
    QPointF pixelAspect;
    qreal rotation;
} Transform;

class Canvas : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage)
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_ENUMS(image transform)

public:
    explicit Canvas(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    Image *image() const;
    Transform transform() const;

public slots:
    void setImage(Image *image);
    void setTransform(Transform arg);
    void updateImage(const QRegion &region);

signals:
    void transformChanged(Transform arg);
    void clicked(const QPoint &position);
    void dragged(const QPoint &a, const QPoint &b);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    Image *m_image;
    Transform m_transform;
    QTransform matrix;
    QTransform inverseMatrix;
    QPoint lastMousePos;
    QPointF lastMouseImagePos;
    bool panKeyDown;

    void updateMatrix();
};

#endif // CANVAS_H
