#include "canvas.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>

std::unique_ptr<QPixmap> Canvas::backgroundPattern = nullptr;

QPixmap *generateBackgroundPattern()
{
    QPixmap *pattern = new QPixmap(32, 32);
    QPainter painter(pattern);
    const QColor base = QColor(127, 127, 127), light = base.lighter(125), dark = base.darker(125);
    painter.fillRect(QRect(0, 0, pattern->width() / 2, pattern->height() / 2), light);
    painter.fillRect(QRect(pattern->width() / 2, 0, pattern->width() - (pattern->width() / 2), pattern->height() / 2), dark);
    painter.fillRect(QRect(0, pattern->height() / 2, pattern->width() / 2, pattern->height() - (pattern->height() / 2)), dark);
    painter.fillRect(QRect(pattern->width() / 2, pattern->height() / 2, pattern->width() - (pattern->width() / 2), pattern->height() - (pattern->height() / 2)), light);
    return pattern;
}

Canvas::Canvas(QWidget *parent) :
    QWidget(parent), m_image(0), m_transform(), panKeyDown(false)
{
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setImage(0);
    if (!backgroundPattern) {
        backgroundPattern.reset(generateBackgroundPattern());
    }
}

void Canvas::resizeEvent(QResizeEvent *)
{
    updateMatrix();
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_image) {
        if (!m_tiled) {
            QRectF imageBounds = inverseMatrix.mapRect(QRectF(event->rect().topLeft(), event->rect().bottomRight() + QPoint(1, 1)));
            QRect imageRect = QRect(QPoint((int)floor(imageBounds.left()), (int)floor(imageBounds.top())),
                                    QPoint((int)ceil(imageBounds.right()), (int)ceil(imageBounds.bottom()))).intersected(m_image->data().rect());
            painter.fillRect(rect(), QBrush(*backgroundPattern));
            painter.setTransform(matrix);
            painter.drawImage(imageRect, m_image->data(), imageRect);
        }
        else {
            QBrush brush = QBrush(m_image->data());
            brush.setTransform(matrix);
            painter.fillRect(rect(), brush);
        }
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
        Transform transform = m_transform;
        transform.pan += delta;
        setTransform(transform);
        lastMousePos = event->pos();
        lastMouseImagePos = mouseImagePosition;
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
            QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            emit clicked(pixel);
            event->accept();
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (m_image) {
            QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            m_image->pick(pixel);
        }
    }
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::SHIFT) {
        Transform transform = m_transform;
        transform.rotation += event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        setTransform(transform);
        event->accept();
    }
    else {
        Transform transform = m_transform;
        transform.zoom *= event->angleDelta().y() > 0 ? 2 : (event->angleDelta().y() < 0 ? .5 : 1);
        setTransform(transform);
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

bool Canvas::tiled() const
{
    return m_tiled;
}

void Canvas::setImage(Image *const image)
{
    m_image = image;
    m_transform.pan = QPointF(0, 0);
    m_transform.zoom = 1.;
    m_transform.pixelAspect = QPointF(1., 1.);
    m_transform.rotation = 0.;
    if (image) {
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        m_transform.pan = -QPointF((qreal)m_image->data().width() / 2., (qreal)m_image->data().height() / 2.);
    }
    else {
        setAttribute(Qt::WA_OpaquePaintEvent, false);
        setAttribute(Qt::WA_NoSystemBackground, false);
    }
    updateMatrix();
    emit transformChanged(m_transform);
}

inline qreal fsel(const qreal a, const qreal b, const qreal c) {
  return a >= 0 ? b : c;
}

inline qreal clamp(qreal value, const qreal min, const qreal max)
{
   value = fsel(value - min, value, min);
   return fsel(value - max, max, value);
}

inline qreal wrap(qreal value, const qreal min, const qreal max)
{
    const qreal range = max - min;
    const qreal  quotient = (value - min) / range;
    const qreal  remainder = quotient - floor(quotient);
    return remainder * range + min;
}

void Canvas::setTransform(const Transform &transform, const bool limit)
{
    if (m_transform.pan != transform.pan ||
            m_transform.zoom != transform.zoom ||
            m_transform.pixelAspect != transform.pixelAspect ||
            m_transform.rotation != transform.rotation) {
        m_transform = transform;
        if (limit) {
            if (m_image) {
                m_transform.pan.setX(clamp(m_transform.pan.x(), (qreal)-m_image->data().width(), 0.));
                m_transform.pan.setY(clamp(m_transform.pan.y(), (qreal)-m_image->data().height(), 0.));
            }
            m_transform.zoom = clamp(m_transform.zoom, 1./16., 256.);
            m_transform.pixelAspect.setX(clamp(m_transform.pixelAspect.x(), 1./16., 16.));
            m_transform.pixelAspect.setY(clamp(m_transform.pixelAspect.y(), 1./16., 16.));
            m_transform.rotation = wrap(m_transform.rotation, 0., 360.);
        }
        updateMatrix();
        emit transformChanged(m_transform);
    }
}

void Canvas::updateImage(const QRegion &region)
{
//    update(matrix.map(region)); // Too slow
    if (!m_tiled) {
        update(matrix.mapRect(region.boundingRect()));
    }
    else {
        update();
    }
}

void Canvas::setTiled(bool tiled)
{
    if (m_tiled != tiled) {
        m_tiled = tiled;
        update();
        emit tiledChanged(tiled);
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
    update();
}

