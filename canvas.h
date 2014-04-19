#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

signals:

public slots:
    void setImage(QImage *image);

private:
    QImage *image;
    QPointF pan;
    qreal scale;
    QPointF pixelAspect;
    qreal rotation;
    QTransform matrix;
    QTransform inverseMatrix;
    QPoint lastMousePos;

    void updateMatrix();
};

#endif // CANVAS_H
