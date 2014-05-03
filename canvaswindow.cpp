#include "canvaswindow.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "application.h"
#include "util.h"
#include "transform.h"
#include <QOpenGLShaderProgram>

CanvasWindow::CanvasWindow(QOpenGLContext *const shareContext) :
    OpenGLWindow(), m_context(), m_image(0), m_transform(), m_tiled(false), panKeyDown(false), m_showFrame(false)
{
//    setFocusPolicy(Qt::WheelFocus);
//    setMouseTracking(true);
    Application *app = (Application *)qApp;
    m_context.setShareContext(&app->context());
    m_context.setFormat(app->format());
    m_context.create();
    setImage(0);
}

void CanvasWindow::initialize()
{
}

void CanvasWindow::render()
{
    initializeOpenGLFunctions();
    m_context.makeCurrent(this);
    glEnable(GL_MULTISAMPLE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.25, 0.25, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    if (m_image) {
        QImage image(m_image->data());

        switch (image.format()) {
        case QImage::Format_Invalid:
            return;
        case QImage::Format_Indexed8:
        case QImage::Format_ARGB32:
            break;
        case QImage::Format_Mono:
        case QImage::Format_MonoLSB:
            image = image.convertToFormat(QImage::Format_Indexed8);
            break;
        default:
            image = image.convertToFormat(QImage::Format_ARGB32);
            break;
        }

        ImageDataFormat format;
        switch (image.format()) {
        case QImage::Format_Indexed8:
            format = ImageDataFormat::Indexed;
            break;
        case QImage::Format_ARGB32:
            format = ImageDataFormat::RGBA;
            break;
        default:
            return;
        }

        ImageData data(image.size(), ImageDataFormat::Indexed, image.bits());

        QOpenGLShaderProgram program;
        program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/image.vert");
        program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/image.frag");
//        program.bindAttributeLocation();
        program.link();
//        qDebug() << program.log();
        glUseProgram(program.programId());
        glUseProgram(0);
    }
}

void CanvasWindow::render(QPainter *painter)
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
    if (m_image) {
        if (!m_tiled || m_showAlpha) {
            painter->fillRect(rect(), QBrush(*canvasBackgroundPixmap));
        }
        if (!m_showAlpha) {
            painter->setCompositionMode(QPainter::CompositionMode_Source);
        }
        else {
            painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        }
        if (!m_tiled) {
            painter->save();
            QRectF imageBounds = m_transform.inverseMatrix().mapRect(QRectF(rect().topLeft(), rect().bottomRight() + QPoint(1, 1)));
            QRect imageRect = QRect(QPoint((int)floor(imageBounds.left()), (int)floor(imageBounds.top())),
                                    QPoint((int)ceil(imageBounds.right()), (int)ceil(imageBounds.bottom()))).intersected(m_image->data().rect());
            painter->setTransform(m_transform.matrix());
            painter->drawImage(imageRect, m_image->data(), imageRect);
//            painter->drawImage(QPoint(0, 0), m_image->data());
            painter->restore();
        }
        else {
            painter->save();
            QBrush brush = QBrush(m_image->data());
            brush.setTransform(m_transform.matrix());
            painter->fillRect(rect(), brush);
            painter->restore();
        }
        if (m_showFrame) {
            painter->save();
            painter->setTransform(m_transform.matrix());
            QPen pen;
            pen.setWidth(0);
            pen.setColor(Qt::white);
            painter->setPen(pen);
            painter->drawRect(m_image->data().rect());
            painter->restore();
        }
    }
}

void CanvasWindow::resizeEvent(QResizeEvent *event)
{
    m_transform.setOrigin(QPoint(event->size().width() / 2, event->size().height() / 2));
    update();
    OpenGLWindow::resizeEvent(event);
    glViewport(0, 0, (GLint)event->size().width(), (GLint)event->size().height());
}

void CanvasWindow::mousePressEvent(QMouseEvent *event)
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

void CanvasWindow::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));
    const QPoint lastPixel = QPoint(floor(lastMouseImagePos.x()), floor(lastMouseImagePos.y()));
    const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
    if (rect().contains(event->pos())) {
//        setFocus();
        if (m_image && pixel != lastPixel && m_image->data().rect().contains(pixel)) {
            emit mousePixelChanged(pixel, m_image->data().pixel(pixel), m_image->isIndexed() ? m_image->data().pixelIndex(pixel) : -1);
        }
    }
    else {
        emit mousePixelChanged(QPoint(0, 0), qRgba(0, 0, 0, 0), false);
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
        const QPointF delta = mouseImagePosition - m_transform.inverseMatrix().map(QPointF(lastMousePos));
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

void CanvasWindow::mouseReleaseEvent(QMouseEvent *event)
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

void CanvasWindow::wheelEvent(QWheelEvent *event)
{
    const bool transformAroundCursor = true;
    if (event->modifiers() & Qt::SHIFT) {
        Transform transform = m_transform;
        const qreal angle = event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        transform.setRotation(transform.rotation() + angle);
        if (transformAroundCursor) {
            const QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));
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
            const QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));

        }
        setTransform(transform);
        event->accept();
    }
}

void CanvasWindow::tabletEvent(QTabletEvent *event)
{
    qDebug() << event;
    if (false) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void CanvasWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        panKeyDown = true;
        lastMousePos = mapFromGlobal(QCursor::pos());
//        grabMouse();
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        event->accept();
    }
    else {
        event->ignore();
    }
}

void CanvasWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        panKeyDown = false;
//        releaseMouse();
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void CanvasWindow::enterEvent(QEvent *const event)
{
    if (m_image) {
        emit mouseEntered();
    }
}

void CanvasWindow::leaveEvent(QEvent *const event)
{
    emit mouseLeft();
}

Image *CanvasWindow::image() const
{
    return m_image;
}

Transform CanvasWindow::transform() const
{
    return m_transform;
}

bool CanvasWindow::tiled() const
{
    return m_tiled;
}

bool CanvasWindow::showFrame() const
{
    return m_showFrame;
}

bool CanvasWindow::showAlpha() const
{
    return m_showAlpha;
}

QRect CanvasWindow::rect() const
{
    return QRect(0, 0, width(), height());
}

void CanvasWindow::setImage(Image *const image)
{
    m_image = image;
    Transform transform;
    transform.setOrigin(QPointF(width() / 2, height() / 2));
    transform.setPan(QPointF(0, 0));
    transform.setZoom(1.);
    transform.setPixelAspect(QPointF(1., 1.));
    transform.setRotation(0.);
    if (image) {
//        setAttribute(Qt::WA_OpaquePaintEvent, true);
//        setAttribute(Qt::WA_NoSystemBackground, true);
        transform.setPan(-QPointF((qreal)m_image->data().width() / 2., (qreal)m_image->data().height() / 2.));
    }
    else {
//        setAttribute(Qt::WA_OpaquePaintEvent, false);
//        setAttribute(Qt::WA_NoSystemBackground, false);
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

void CanvasWindow::setTransform(const Transform &transform, const bool limit)
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
        update();
        emit transformChanged(m_transform);
    }
}

void CanvasWindow::updateImage(const QRegion &region)
{
//    update(matrix.map(region)); // Too slow
    if (!m_tiled) {
//        update(m_transform.matrix().mapRect(region.boundingRect()));
        update();
    }
    else {
        update();
    }
}
void CanvasWindow::setTiled(const bool tiled)
{
    if (m_tiled != tiled) {
        m_tiled = tiled;
        update();
        emit tiledChanged(tiled);
    }
}

void CanvasWindow::setShowFrame(const bool showFrame)
{
    if (m_showFrame != showFrame) {
        m_showFrame = showFrame;
        update();
        emit showFrameChanged(showFrame);
    }
}

void CanvasWindow::setShowAlpha(bool showAlpha)
{
    if (m_showAlpha != showAlpha) {
        m_showAlpha = showAlpha;
        update();
        emit showAlphaChanged(showAlpha);
    }
}

void CanvasWindow::update() {
    renderLater();
}
