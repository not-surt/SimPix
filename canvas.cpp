#include "canvas.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>

Canvas::Canvas(QWidget *parent) :
    QWidget(parent)
{
    setImage(0);
}

void Canvas::setImage(QImage *image)
{
    this->image = image;
    pan = QPointF(0, 0);
    scale = 1.;
    pixelAspect = QPointF(2., 1.);
    rotation = 0.;
    updateMatrix();
}

void Canvas::resizeEvent(QResizeEvent *)
{
    updateMatrix();
}

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (image) {
        painter.fillRect(rect(), Qt::gray);
        painter.setTransform(matrix);
        painter.drawImage(QRectF(image->rect()), *image, QRectF(image->rect()));
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton or (event->buttons() & Qt::LeftButton and event->modifiers() & Qt::CTRL)) {
        lastMousePos = event->pos();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    QPointF mousePosition = inverseMatrix.map(QPointF(event->pos()));
    if (event->buttons() & Qt::MiddleButton or (event->buttons() & Qt::LeftButton and event->modifiers() & Qt::CTRL)) {
        QPointF delta = mousePosition - inverseMatrix.map(QPointF(lastMousePos));
        qDebug() << pan << " +" << delta;
        pan += delta;
        lastMousePos = event->pos();
        updateMatrix();
        update();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::SHIFT) {
        rotation += event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        updateMatrix();
        update();
        event->accept();
    }
    else {
        scale *= event->angleDelta().y() > 0 ? 2 : (event->angleDelta().y() < 0 ? .5 : 1);
        updateMatrix();
        update();
        event->accept();
    }
}

void Canvas::updateMatrix()
{
    matrix = QTransform();
    matrix.translate(width() / 2, height() / 2);
    matrix.rotate(rotation);
    matrix.scale(scale * pixelAspect.x(), scale * pixelAspect.y());
    matrix.translate(pan.x(), pan.y());
    inverseMatrix = matrix.inverted();
}
