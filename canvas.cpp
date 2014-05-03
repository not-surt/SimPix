#include "canvas.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include <QApplication>
#include "util.h"
#include "transform.h"

Canvas::Canvas(QWidget *parent) :
    QWidget(parent), m_image(0), m_transform(), m_tiled(false), panKeyDown(false), m_showFrame(false)
{
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setImage(0);
}

void Canvas::resizeEvent(QResizeEvent *event)
{
    m_transform.setOrigin(QVector3D(event->size().width() / 2, event->size().height() / 2, 0));
    update();
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
            QRectF imageBounds = m_transform.inverseMatrix().mapRect(QRectF(event->rect().topLeft(), event->rect().bottomRight() + QPoint(1, 1)));
            QRect imageRect = QRect(QPoint((int)floor(imageBounds.left()), (int)floor(imageBounds.top())),
                                    QPoint((int)ceil(imageBounds.right()), (int)ceil(imageBounds.bottom()))).intersected(m_image->data().rect());
            painter.setTransform(m_transform.matrix().toTransform());
            painter.drawImage(imageRect, m_image->data(), imageRect);
            painter.restore();
        }
        else {
            painter.save();
            QBrush brush = QBrush(m_image->data());
            brush.setTransform(m_transform.matrix().toTransform());
            painter.fillRect(rect(), brush);
            painter.restore();
        }
        if (m_showFrame) {
            painter.save();
            painter.setTransform(m_transform.matrix().toTransform());
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
    lastMouseImagePos = m_transform.inverseMatrix().map(QPointF(event->pos()));
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
    const QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));
    const QPoint lastPixel = QPoint(floor(lastMouseImagePos.x()), floor(lastMouseImagePos.y()));
    const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
    if (rect().contains(event->pos())) {
        setFocus();
        if (m_image && pixel != lastPixel && m_image->data().rect().contains(pixel)) {
            emit mousePixelChanged(pixel, m_image->data().pixel(pixel), m_image->isIndexed() ? m_image->data().pixelIndex(pixel) : -1);
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
        const QVector3D delta = QVector3D(mouseImagePosition) - m_transform.inverseMatrix().map(QVector3D(lastMousePos));
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
    QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));
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
            Image::ContextColour context = (event->modifiers() & Qt::SHIFT) ? Image::Secondary : Image::Primary;
            m_image->pick(pixel, context);
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
        const qreal angle = event->angleDelta().y() > 0 ? 15.f : (event->angleDelta().y() < 0 ? -15.f : 0.f);
        transform.setRotation(transform.rotation() + angle);
        if (transformAroundCursor) {
            const QVector3D mouseImagePosition = m_transform.inverseMatrix().map(QVector3D(event->pos()));
            const QVector3D mouseDelta = mouseImagePosition - transform.pan();
            qDebug() << mouseDelta;
        }
        setTransform(transform);
        event->accept();
    }
    else {
        Transform transform = m_transform;
        const qreal scale = event->angleDelta().y() > 0 ? 2.f : (event->angleDelta().y() < 0 ? .5f : 1.f);
        transform.setZoom(transform.zoom() * scale);
        if (transformAroundCursor) {
            const QVector3D mouseImagePosition = m_transform.inverseMatrix().map(QVector3D(event->pos()));

        }
        setTransform(transform);
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
    transform.setOrigin(QVector3D(width() / 2, height() / 2, 0.f));
    transform.setPan(QVector3D(0.f, 0.f, 0.f));
    transform.setZoom(1.f);
    transform.setPixelAspect(QVector3D(1.f, 1.f, 0.f));
    transform.setRotation(0.f);
    if (image) {
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        transform.setPan(-QVector3D((qreal)m_image->data().width() / 2.f, (qreal)m_image->data().height() / 2.f, 0.f));
    }
    else {
        setAttribute(Qt::WA_OpaquePaintEvent, false);
        setAttribute(Qt::WA_NoSystemBackground, false);
    }
    setTransform(transform);
}

void Canvas::setTransform(const Transform &transform, const bool limit)
{
    if (m_transform != transform) {
        m_transform = transform;
        if (limit) {
            if (m_image) {
                if (!m_tiled) {
                    m_transform.setPan(QVector3D(clamp(m_transform.pan().x(), (qreal)-m_image->data().width(), 0.f),
                                                 clamp(m_transform.pan().y(), (qreal)-m_image->data().height(), 0.f),
                                                 0.f));
                }
                else {
                    m_transform.setPan(QVector3D(wrap(m_transform.pan().x(), (qreal)-m_image->data().width(), 0.f),
                                                 wrap(m_transform.pan().y(), (qreal)-m_image->data().height(), 0.f),
                                                 0.f));
                }
            }
            m_transform.setZoom(clamp(m_transform.zoom(), 1.f/16.f, 256.f));
            m_transform.setPixelAspect(QVector3D(clamp(m_transform.pixelAspect().x(), 1.f/16.f, 16.f), clamp(m_transform.pixelAspect().y(), 1.f/16.f, 16.f), 0.f));
            m_transform.setRotation(wrap(m_transform.rotation(), 0.f, 360.f));
        }
        update();
        emit transformChanged(m_transform);
    }
}

void Canvas::updateImage(const QRegion &region)
{
//    update(matrix.map(region)); // Too slow
    if (!m_tiled) {
        update(m_transform.matrix().mapRect(region.boundingRect()));
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
