#include "canvas.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>

Canvas::Canvas(QWidget *parent) :
    QWidget(parent), m_image(0), m_transform(), panKeyDown(false)
{
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
//    setAttribute(Qt::WA_OpaquePaintEvent, true);
//    setAttribute(Qt::WA_NoSystemBackground, true);
    setImage(0);
}

void Canvas::resizeEvent(QResizeEvent *)
{
    updateMatrix();
}

void qRectExpand(QRectF &rect, const QPointF &point)
{
    if (rect.isEmpty()) {
        rect.setSize(QSizeF(0., 0.));
        rect.setTopLeft(point);
    }
    else {
        if (rect.left() > point.x()) rect.setLeft(point.x());
        if (rect.right() < point.x()) rect.setRight(point.x());
        if (rect.top() > point.y()) rect.setTop(point.y());
        if (rect.bottom() < point.y()) rect.setBottom(point.y());
    }
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_image) {
        QRectF imageBounds = inverseMatrix.mapRect(QRectF(event->rect().topLeft(), event->rect().bottomRight() + QPoint(1, 1)));
        QRect imageRect = QRect(QPoint((int)floor(imageBounds.left()), (int)floor(imageBounds.top())),
                                QPoint((int)ceil(imageBounds.right()), (int)ceil(imageBounds.bottom()))).intersected(m_image->data().rect());
        painter.fillRect(rect(), Qt::gray);
        painter.setTransform(matrix);
        painter.drawImage(imageRect, m_image->data(), imageRect);
//        painter.setPen(Qt::black);
//        painter.drawRect(imageRect);
//        qDebug() << imageRect;
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
    lastMouseImagePos = inverseMatrix.map(QPointF(event->pos()));
    if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
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
    QPointF mouseImagePosition = inverseMatrix.map(QPointF(event->pos()));
    if (!panKeyDown && event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        if (m_image) {
            QPoint lastPixel = QPoint(floor(lastMouseImagePos.x()), floor(lastMouseImagePos.y()));
            QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            emit dragged(lastPixel, pixel);
            lastMousePos = event->pos();
            lastMouseImagePos = mouseImagePosition;
            event->accept();
        }
    }
    else if (event->buttons() & Qt::RightButton) {

    }
    else if (panKeyDown || event->buttons() & Qt::MiddleButton || (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        QPointF delta = mouseImagePosition - inverseMatrix.map(QPointF(lastMousePos));
//        qDebug() << m_transform.pan << " +" << delta;
        m_transform.pan += delta;
        emit transformChanged(m_transform);
        lastMousePos = event->pos();
        lastMouseImagePos = mouseImagePosition;
        updateMatrix();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF mouseImagePosition = inverseMatrix.map(QPointF(event->pos()));
    if (event->button() == Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        if (m_image) {
            QPoint lastPixel = QPoint(floor(lastMouseImagePos.x()), floor(lastMouseImagePos.y()));
            QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            emit clicked(pixel);
            event->accept();
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (m_image) {
            m_image->setPrimaryColour(0);
        }
    }
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::SHIFT) {
        m_transform.rotation += event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        emit transformChanged(m_transform);
        updateMatrix();
        event->accept();
    }
    else {
        m_transform.zoom *= event->angleDelta().y() > 0 ? 2 : (event->angleDelta().y() < 0 ? .5 : 1);
        emit transformChanged(m_transform);
        updateMatrix();
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
        emit transformChanged(arg);
    }
}

void Canvas::updateImage(const QRegion &region)
{
//    update(matrix.map(region)); // Too slow
    update(matrix.mapRect(region.boundingRect()));
}

void Canvas::updateMatrix()
{
    matrix = QTransform();
    matrix.translate(width() / 2, height() / 2);
    matrix.rotate(m_transform.rotation);
    matrix.scale(m_transform.zoom * m_transform.pixelAspect.x(), m_transform.zoom * m_transform.pixelAspect.y());
    matrix.translate(m_transform.pan.x(), m_transform.pan.y());
    inverseMatrix = matrix.inverted();
    update();
}

