#include "canvas.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include <QApplication>
#include "util.h"
#include "transform.h"

std::unique_ptr<QPixmap> Canvas::backgroundPattern = nullptr;

Canvas::Canvas(QWidget *parent) :
    QWidget(parent), m_image(0), m_transform(), m_tiled(false), panKeyDown(false), m_showFrame(false)
{
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setImage(0);
}

void Canvas::resizeEvent(QResizeEvent *event)
{
    m_transform.setOrigin(QPoint(event->size().width() / 2, event->size().height() / 2));
    updateMatrix();
    QWidget::resizeEvent(event);
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    if (m_image) {
        if (!m_tiled || m_showAlpha) {
            painter.fillRect(rect(), QBrush(*canvasBackgroundPixmap));
        }
        if (!m_showAlpha) {
            painter.setCompositionMode(QPainter::CompositionMode_Source);
        }
        else {
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }
        if (!m_tiled) {
            painter.save();
            QRectF imageBounds = inverseMatrix.mapRect(QRectF(event->rect().topLeft(), event->rect().bottomRight() + QPoint(1, 1)));
            QRect imageRect = QRect(QPoint((int)floor(imageBounds.left()), (int)floor(imageBounds.top())),
                                    QPoint((int)ceil(imageBounds.right()), (int)ceil(imageBounds.bottom()))).intersected(m_image->data().rect());
            painter.setTransform(matrix);
            painter.drawImage(imageRect, m_image->data(), imageRect);
            painter.restore();
        }
        else {
            painter.save();
            QBrush brush = QBrush(m_image->data());
            brush.setTransform(matrix);
            painter.fillRect(rect(), brush);
            painter.restore();
        }
        if (m_showFrame) {
            painter.save();
            painter.setTransform(matrix);
            QPen pen;
            pen.setWidth(0);
            pen.setColor(Qt::white);
            painter.setPen(pen);
            painter.drawRect(m_image->data().rect());
            painter.restore();
        }
//        qDebug() << imageRect;
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
    lastMouseImagePos = inverseMatrix.map(QPointF(event->pos()));
    if (event->button() == Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        QApplication::setOverrideCursor(Qt::CrossCursor);
        event->accept();
    }
    else if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        event->accept();
    }
    else if (event->button() == Qt::RightButton) {
        QApplication::setOverrideCursor(Qt::PointingHandCursor);
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF mouseImagePosition = inverseMatrix.map(QPointF(event->pos()));
    const QPoint lastPixel = QPoint(floor(lastMouseImagePos.x()), floor(lastMouseImagePos.y()));
    const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
    if (rect().contains(event->pos())) {
        setFocus();
        if (m_image && pixel != lastPixel && m_image->data().rect().contains(pixel)) {
            emit pixelChanged(pixel, m_image->data().pixel(pixel), m_image->isIndexed() ? m_image->data().pixelIndex(pixel) : -1);
        }
    }
    if (!panKeyDown && event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        if (m_image) {
            emit dragged(lastPixel, pixel);
            lastMousePos = event->pos();
            lastMouseImagePos = mouseImagePosition;
            event->accept();
        }
    }
    else if (event->buttons() & Qt::RightButton) {

    }
    else if (panKeyDown || event->buttons() & Qt::MiddleButton || (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        const QPointF delta = mouseImagePosition - inverseMatrix.map(QPointF(lastMousePos));
        Transform transform = m_transform;
        transform.setPan(transform.pan() + delta);
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
            const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            emit clicked(pixel);
        }
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else if (event->button() == Qt::RightButton) {
        if (m_image) {
            const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            m_image->pick(pixel);
        }
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    const bool transformAroundCursor = true;
    if (event->modifiers() & Qt::SHIFT) {
        Transform transform = m_transform;
        const qreal angle = event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        transform.setRotation(transform.rotation() + angle);
        if (transformAroundCursor) {
            const QPointF mouseImagePosition = inverseMatrix.map(QPointF(event->pos()));
            const QPointF mouseDelta = mouseImagePosition - transform.pan();
            qDebug() << mouseDelta;
        }
        setTransform(transform);
        event->accept();
    }
    else {
        Transform transform = m_transform;
        const qreal scale = event->angleDelta().y() > 0 ? 2 : (event->angleDelta().y() < 0 ? .5 : 1);
        transform.setZoom(transform.zoom() * scale);
        if (transformAroundCursor) {
            const QPointF mouseImagePosition = inverseMatrix.map(QPointF(event->pos()));

        }
        setTransform(transform);
        updateMatrix();
        event->accept();
    }
}

void Canvas::tabletEvent(QTabletEvent *event)
{
    qDebug() << event;
    if (false) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        panKeyDown = true;
        lastMousePos = mapFromGlobal(QCursor::pos());
        grabMouse();
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
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
        QApplication::restoreOverrideCursor();
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

bool Canvas::showFrame() const
{
    return m_showFrame;
}

bool Canvas::showAlpha() const
{
    return m_showAlpha;
}

void Canvas::setImage(Image *const image)
{
    m_image = image;
    Transform transform;
    transform.setOrigin(QPointF(width() / 2, height() / 2));
    transform.setPan(QPointF(0, 0));
    transform.setZoom(1.);
    transform.setPixelAspect(QPointF(1., 1.));
    transform.setRotation(0.);
    if (image) {
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        transform.setPan(-QPointF((qreal)m_image->data().width() / 2., (qreal)m_image->data().height() / 2.));
    }
    else {
        setAttribute(Qt::WA_OpaquePaintEvent, false);
        setAttribute(Qt::WA_NoSystemBackground, false);
    }
    setTransform(transform);
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
    if (m_transform != transform) {
        m_transform = transform;
        if (limit) {
            if (m_image) {
                if (!m_tiled) {
                    m_transform.setPan(QPointF(clamp(m_transform.pan().x(), (qreal)-m_image->data().width(), 0.),
                                               clamp(m_transform.pan().y(), (qreal)-m_image->data().height(), 0.)));
                }
                else {
                    m_transform.setPan(QPointF(wrap(m_transform.pan().x(), (qreal)-m_image->data().width(), 0.),
                                               wrap(m_transform.pan().y(), (qreal)-m_image->data().height(), 0.)));
                }
            }
            m_transform.setZoom(clamp(m_transform.zoom(), 1./16., 256.));
            m_transform.setPixelAspect(QPointF(clamp(m_transform.pixelAspect().x(), 1./16., 16.), clamp(m_transform.pixelAspect().y(), 1./16., 16.)));
            m_transform.setRotation(wrap(m_transform.rotation(), 0., 360.));
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

void Canvas::setTiled(const bool tiled)
{
    if (m_tiled != tiled) {
        m_tiled = tiled;
        update();
        emit tiledChanged(tiled);
    }
}

void Canvas::setShowFrame(const bool showFrame)
{
    if (m_showFrame != showFrame) {
        m_showFrame = showFrame;
        update();
        emit showFrameChanged(showFrame);
    }
}

void Canvas::setShowAlpha(bool showAlpha)
{
    if (m_showAlpha != showAlpha) {
        m_showAlpha = showAlpha;
        update();
        emit showAlphaChanged(showAlpha);
    }
}

void Canvas::updateMatrix()
{
    matrix = QTransform();
    matrix.translate(m_transform.origin().x(), m_transform.origin().y());
    matrix.rotate(m_transform.rotation());
    matrix.scale(m_transform.zoom() * m_transform.pixelAspect().x(), m_transform.zoom() * m_transform.pixelAspect().y());
    matrix.translate(m_transform.pan().x(), m_transform.pan().y());
    inverseMatrix = matrix.inverted();
    update();
}


void Canvas::enterEvent(QEvent *const event)
{
    if (m_image) {
        emit mouseEntered();
    }
    QWidget::enterEvent(event);
}

void Canvas::leaveEvent(QEvent *const event)
{
    emit mouseLeft();
    QWidget::leaveEvent(event);
}
