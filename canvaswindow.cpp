#include "canvaswindow.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "application.h"
#include "util.h"
#include "transform.h"
#include <QOpenGLShaderProgram>
#include <QGLWidget>

CanvasWindow::CanvasWindow(QOpenGLContext *const shareContext) :
    OpenGLWindow(), m_context(), m_image(0), m_transform(), m_tiled(false), panKeyDown(false), m_showFrame(false), matricesDirty(true), m_vertexBuffer(0)
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
//    glEnable(GL_MULTISAMPLE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
//    glClearColor(0.25, 0.25, 0.5, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT);

    m_context.makeCurrent(this);
    initializeOpenGLFunctions();

    if (!m_tiled || m_showAlpha) {
        glActiveTexture(GL_TEXTURE0 + 0);
        GLuint program = app->program("checkerboard");
        glUseProgram(program);
        GLint positionAttrib = glGetAttribLocation(program, "position");
        GLint matrixUniform = glGetUniformLocation(program, "matrix");
        GLint textureMatrixUniform = glGetUniformLocation(program, "textureMatrix");
        GLint sizeUniform = glGetUniformLocation(program, "size");
        GLint colour0Uniform = glGetUniformLocation(program, "colour0");
        GLint colour1Uniform = glGetUniformLocation(program, "colour1");
        glUniform2f(sizeUniform, 32.f, 32.f);
        glUniform4i(colour0Uniform, 127, 127, 127, 255);
        glUniform4i(colour1Uniform, 95, 95, 95, 255);
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, this->matrix().constData());
        glUniformMatrix4fv(textureMatrixUniform, 1, GL_FALSE, QMatrix4x4().constData());
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (m_image) {
        m_context.makeCurrent(this);
        initializeOpenGLFunctions();

        if (m_showAlpha) {
            glEnable(GL_BLEND);
        }
        else {
            glDisable(GL_BLEND);
        }


        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, m_image->imageData()->texture());

        if (m_image->paletteData()) {
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, m_image->paletteData()->texture());
        }

        GLuint program = app->program("image");
        glUseProgram(program);

        GLint positionAttrib = glGetAttribLocation(program, "position");

        GLint textureUnitUniform = glGetUniformLocation(program, "textureUnit");

        GLint isIndexedUniform = glGetUniformLocation(program, "isIndexed");
        GLint hasPaletteUniform = glGetUniformLocation(program, "hasPalette");
        GLint paletteTextureUnitUniform = glGetUniformLocation(program, "paletteTextureUnit");

        GLint matrixUniform = glGetUniformLocation(program, "matrix");
        GLint textureMatrixUniform = glGetUniformLocation(program, "textureMatrix");


        glUniform1i(textureUnitUniform, 0);

        glUniform1i(isIndexedUniform, (m_image->imageData()->format() == ImageDataFormat::Indexed));
        glUniform1i(hasPaletteUniform, (m_image->paletteData() != nullptr));
        glUniform1i(paletteTextureUnitUniform, 1);

        if (m_tiled) {
            glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, this->matrix().constData());
            glUniformMatrix4fv(textureMatrixUniform, 1, GL_FALSE, m_transform.inverseMatrix().constData());

            glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glDisableVertexAttribArray(positionAttrib);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else {
            glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, (this->matrix() * m_transform.matrix()).constData());
            glUniformMatrix4fv(textureMatrixUniform, 1, GL_FALSE, QMatrix4x4().constData());

            glBindBuffer(GL_ARRAY_BUFFER, m_image->imageData()->vertexBuffer());
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glDisableVertexAttribArray(positionAttrib);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        glUseProgram(0);

        if (m_showFrame) {
            GLuint program = app->program("frame");
            glUseProgram(program);
            GLint matrixUniform = glGetUniformLocation(program, "matrix");
            GLint textureMatrixUniform = glGetUniformLocation(program, "textureMatrix");
            GLint colourUniform = glGetUniformLocation(program, "colour");
            glUniform4i(colourUniform, 255, 255, 255, 127);
            glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, (this->matrix() * m_transform.matrix()).constData());
            glUniformMatrix4fv(textureMatrixUniform, 1, GL_FALSE, QMatrix4x4().constData());

            GLint positionAttrib = glGetAttribLocation(program, "position");
            glBindBuffer(GL_ARRAY_BUFFER, m_image->imageData()->vertexBuffer());
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glDrawArrays(GL_LINE_LOOP, 0, 4);

            glDisableVertexAttribArray(positionAttrib);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

//        m_context.doneCurrent(); // why crash?
    }
}

void CanvasWindow::resizeEvent(QResizeEvent *event)
{
    m_context.makeCurrent(this);
    initializeOpenGLFunctions();
    glViewport(0, 0, (GLint)event->size().width(), (GLint)event->size().height());
    glDeleteBuffers((GLsizei)1, &m_vertexBuffer);
    glGenBuffers((GLsizei)1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    const GLfloat vertices[][3] = {
        {0.f, 0.f, 0.f},
        {(GLfloat)event->size().width(), 0.f, 0.f},
        {(GLfloat)event->size().width(), (GLfloat)event->size().height(), 0.f},
        {0.f, (GLfloat)event->size().height(), 0.f},
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    m_context.doneCurrent();
    m_transform.setOrigin(QVector3D(event->size().width() / 2.f, event->size().height() / 2.f, 0.f));
    matricesDirty = true;
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

GLuint CanvasWindow::vertexBuffer() const
{
    return m_vertexBuffer;
}

const QMatrix4x4 &CanvasWindow::matrix()
{
    if (matricesDirty) {
        updateMatrices();
    }
    return m_matrix;
}

const QMatrix4x4 &CanvasWindow::inverseMatrix()
{
    if (matricesDirty) {
        updateMatrices();
    }
    return m_inverseMatrix;
}

void CanvasWindow::updateMatrices()
{
    m_matrix = QMatrix4x4();
    m_matrix.ortho(0., (float)width(), (float)height(), 0.f, -1.f, 1.f);
    m_inverseMatrix = m_matrix.inverted();
    matricesDirty = false;
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
//                    QPointF imageCentreScreenPosition = m_transform.matrix().map(QPointF((float)m_image->data().width() / 2.f, (float)m_image->data().height() / 2.f));
//                    qDebug() << imageCentreScreenPosition;
//                    QVector3D clampedImageCentreScreenPosition = QVector3D(clamp(imageCentreScreenPosition.x(), 0.f, (float)width()),
//                                                                         clamp(imageCentreScreenPosition.y(), 0.f, (float)height()),
//                                                                         0.f);
//                    qDebug() << clampedImageCentreScreenPosition;
//                    m_transform.setPan(m_transform.inverseMatrix().map(clampedImageCentreScreenPosition));
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
