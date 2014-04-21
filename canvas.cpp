#include "canvas.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>

Canvas::Canvas(QWidget *parent) :
    QWidget(parent), m_image(0), m_transform(), panKeyDown(false)
{
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setImage(0);
}

void Canvas::resizeEvent(QResizeEvent *)
{
    updateMatrix();
}

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (m_image) {
        painter.fillRect(rect(), Qt::gray);
        painter.setTransform(matrix);
        painter.drawImage(QRectF(m_image->rect()), *m_image, QRectF(m_image->rect()));
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton || (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        lastMousePos = event->pos();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (rect().contains(event->pos())) {
        setFocus();
    }
    QPointF mousePosition = inverseMatrix.map(QPointF(event->pos()));
    if (!panKeyDown && event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {

    }
    else if (event->buttons() & Qt::RightButton) {

    }
    else if (panKeyDown || event->buttons() & Qt::MiddleButton || (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        QPointF delta = mousePosition - inverseMatrix.map(QPointF(lastMousePos));
//        qDebug() << m_transform.pan << " +" << delta;
        m_transform.pan += delta;
        emit transformChanged(m_transform);
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
        m_transform.rotation += event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        emit transformChanged(m_transform);
        updateMatrix();
        update();
        event->accept();
    }
    else {
        m_transform.zoom *= event->angleDelta().y() > 0 ? 2 : (event->angleDelta().y() < 0 ? .5 : 1);
        emit transformChanged(m_transform);
        updateMatrix();
        update();
        event->accept();
    }
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        panKeyDown = true;
        lastMousePos = mapFromGlobal(QCursor::pos());
        grabMouse();
//        qDebug() << panKeyDown;
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        panKeyDown = false;
//        qDebug() << panKeyDown;
        releaseMouse();
        event->accept();
    }
    else {
        event->ignore();
    }
}

Image *Canvas::image() const
{
    return m_image;
}

Transform Canvas::transform() const
{
    return m_transform;
}

void Canvas::setImage(Image *arg)
{
    m_image = arg;
    m_transform.pan = QPointF(0, 0);
    m_transform.zoom = 1.;
    m_transform.pixelAspect = QPointF(1., 1.);
    m_transform.rotation = 0.;
    updateMatrix();
    update();
    emit transformChanged(m_transform);
}

void Canvas::setTransform(Transform arg)
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

void Canvas::updateMatrix()
{
    matrix = QTransform();
    matrix.translate(width() / 2, height() / 2);
    matrix.rotate(m_transform.rotation);
    matrix.scale(m_transform.zoom * m_transform.pixelAspect.x(), m_transform.zoom * m_transform.pixelAspect.y());
    matrix.translate(m_transform.pan.x(), m_transform.pan.y());
    inverseMatrix = matrix.inverted();
}
