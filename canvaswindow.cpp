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
    Application *const app = (Application *)qApp;
    m_context.setFormat(app->format());
//    m_context.setShareContext(&app->context());
    app->context().setShareContext(&m_context);
    m_context.create();
    qDebug() << "Share: " << m_context.shareContext();
    setImage(0);
}

void CanvasWindow::initialize()
{
}

void CanvasWindow::render()
{
    Application *const app = (Application *)qApp;
    m_context.makeCurrent(this);
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.25, 0.25, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    if (m_image) {

        app->contextMakeCurrent();
        initializeOpenGLFunctions();
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

//        m_context.makeCurrent(this);
//        initializeOpenGLFunctions();
//        glActiveTexture(GL_TEXTURE0 + 0);
//        //glBindSampler(0, app->sampler("Standard"));
//        const GLuint program = app->program("image");
//        const GLuint positionId = glGetAttribLocation(program, "position");
////        const GLuint matrixId = glGetUniformLocation(program, "matrix");
//        glUseProgram(program);
//        GLfloat mat3[9];
//        qTransformFillGlslMat3(transform().matrix(), mat3);
////        glUniformMatrix3fv(matrixId, 1, GL_FALSE, mat3);
//        glBindTexture(GL_TEXTURE_2D, data.texture());
//        glBindBuffer(GL_ARRAY_BUFFER, data.vertexBuffer());
//        glEnableVertexAttribArray(positionId);
//        glVertexAttribPointer(positionId, 2, GL_FLOAT, GL_FALSE, 0, 0);
//        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


////        QOpenGLShaderProgram program;
////        program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/image.vert");
////        program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/image.frag");
//////        program.bindAttributeLocation();
////        program.link();
//////        qDebug() << program.log();
////        glUseProgram(program.programId());
////        glUseProgram(0);

        app->contextMakeCurrent();
        initializeOpenGLFunctions();
        QOpenGLShaderProgram *prog = app->program("image");
        app->contextDoneCurrent();
        m_context.makeCurrent(this);
        initializeOpenGLFunctions();
        prog->bind();

        GLuint vertexLocation = prog->attributeLocation("vertex");
        GLuint matrixLocation = prog->uniformLocation("matrix");
        GLuint colorLocation = prog->uniformLocation("color");
//        const GLfloat triangleVertices[] = {
//            0.f,  0.f,  0.f,
//            (GLfloat)image.size().width(), 0.f, 0.f,
//            0.f,  (GLfloat)image.size().height(), 0.f
//        };

        QColor color(0, 255, 0, 255);

        QMatrix4x4 pmvMatrix;
        pmvMatrix.ortho(rect());
        pmvMatrix *= m_transform.matrix();

//        prog->enableAttributeArray(vertexLocation);
//        prog->setAttributeArray(vertexLocation, triangleVertices, 3);
        prog->setUniformValue(matrixLocation, pmvMatrix);
        prog->setUniformValue(colorLocation, color);

        glBindBuffer(GL_ARRAY_BUFFER, data.vertexBuffer());
        glEnableVertexAttribArray(vertexLocation);
        glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(vertexLocation);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

//        prog->disableAttributeArray(vertexLocation);

//        m_context.doneCurrent(); why stop draw?
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
            painter->setTransform(m_transform.matrix().toTransform());
            painter->drawImage(imageRect, m_image->data(), imageRect);
//            painter->drawImage(QPoint(0, 0), m_image->data());
            painter->restore();
        }
        else {
            painter->save();
            QBrush brush = QBrush(m_image->data());
            brush.setTransform(m_transform.matrix().toTransform());
            painter->fillRect(rect(), brush);
            painter->restore();
        }
        if (m_showFrame) {
            painter->save();
            painter->setTransform(m_transform.matrix().toTransform());
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
    m_context.makeCurrent(this);
    initializeOpenGLFunctions();
    glViewport(0, 0, (GLint)event->size().width(), (GLint)event->size().height());
    m_context.doneCurrent();
    m_transform.setOrigin(QVector3D(event->size().width() / 2.f, event->size().height() / 2.f, 0.f));
    update();
    OpenGLWindow::resizeEvent(event);
}

void CanvasWindow::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
    lastMouseImagePos = m_transform.inverseMatrix().map(QVector3D(event->pos()));
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
    const QVector3D mouseImagePosition = m_transform.inverseMatrix().map(QVector3D(event->pos()));
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
        const QVector3D delta = mouseImagePosition - m_transform.inverseMatrix().map(QVector3D(lastMousePos));
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
            const QVector3D mouseImagePosition = m_transform.inverseMatrix().map(QVector3D(event->pos()));
            const QVector3D mouseDelta = mouseImagePosition - transform.pan();
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
    transform.setOrigin(QVector3D((qreal)floor(width() / 2.f), floor((qreal)height() / 2.f), 0.f));
    transform.setPan(QVector3D(0.f, 0.f, 0.f));
    transform.setZoom(1.f);
    transform.setPixelAspect(QVector3D(1.f, 1.f, 0.f));
    transform.setRotation(0.f);
    if (image) {
//        setAttribute(Qt::WA_OpaquePaintEvent, true);
//        setAttribute(Qt::WA_NoSystemBackground, true);
        transform.setPan(-QVector3D(floor((qreal)m_image->data().width() / 2.f), floor((qreal)m_image->data().height() / 2.f), 0.f));
    }
    else {
//        setAttribute(Qt::WA_OpaquePaintEvent, false);
//        setAttribute(Qt::WA_NoSystemBackground, false);
    }
    setTransform(transform);
}

void CanvasWindow::setTransform(const Transform &transform, const bool limit)
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
