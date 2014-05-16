#include "scenewindow.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "application.h"
#include "util.h"
#include "transform.h"
#include <QOpenGLShaderProgram>
#include <QGLWidget>

SceneWindow::SceneWindow(QOpenGLContext *const shareContext) :
    OpenGLWindow(), m_context(), m_scene(0), m_transform(), m_tiled(false), panKeyDown(false), m_showFrame(false), matricesDirty(true), m_vertexBuffer(0)
{
//    setFocusPolicy(Qt::WheelFocus);
//    setMouseTracking(true);
    m_context.setFormat(*APP->format());
//    m_context.setShareContext(&APP->context());
    APP->context()->setShareContext(&m_context);
    m_context.create();
    qDebug() << "Share: " << m_context.shareContext();
    setImage(0);
}

void SceneWindow::initialize()
{
}

void SceneWindow::render()
{
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
        GLuint program = APP->program("checkerboard");
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

    if (m_scene) {
        m_context.makeCurrent(this);
        initializeOpenGLFunctions();

        if (m_showAlpha) {
            glEnable(GL_BLEND);
        }
        else {
            glDisable(GL_BLEND);
        }


        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, m_scene->imageData()->texture());

        if (m_scene->paletteData()) {
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, m_scene->paletteData()->texture());
        }

        GLuint program = APP->program("image");
        glUseProgram(program);

        GLint positionAttrib = glGetAttribLocation(program, "position");

        GLint textureUnitUniform = glGetUniformLocation(program, "textureUnit");

        GLint isIndexedUniform = glGetUniformLocation(program, "isIndexed");
        GLint hasPaletteUniform = glGetUniformLocation(program, "hasPalette");
        GLint paletteTextureUnitUniform = glGetUniformLocation(program, "paletteTextureUnit");

        GLint matrixUniform = glGetUniformLocation(program, "matrix");
        GLint textureMatrixUniform = glGetUniformLocation(program, "textureMatrix");


        glUniform1i(textureUnitUniform, 0);

        glUniform1i(isIndexedUniform, (m_scene->imageData()->format() == ImageDataFormat::Indexed));
        glUniform1i(hasPaletteUniform, (m_scene->paletteData() != nullptr));
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

            glBindBuffer(GL_ARRAY_BUFFER, m_scene->imageData()->vertexBuffer());
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glDisableVertexAttribArray(positionAttrib);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        glUseProgram(0);

        if (m_showFrame) {
            GLuint program = APP->program("frame");
            glUseProgram(program);
            GLint matrixUniform = glGetUniformLocation(program, "matrix");
            GLint textureMatrixUniform = glGetUniformLocation(program, "textureMatrix");
            GLint colourUniform = glGetUniformLocation(program, "colour");
            glUniform4i(colourUniform, 255, 255, 255, 127);
            glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, (this->matrix() * m_transform.matrix()).constData());
            glUniformMatrix4fv(textureMatrixUniform, 1, GL_FALSE, QMatrix4x4().constData());

            GLint positionAttrib = glGetAttribLocation(program, "position");
            glBindBuffer(GL_ARRAY_BUFFER, m_scene->imageData()->vertexBuffer());
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glDrawArrays(GL_LINE_LOOP, 0, 4);

            glDisableVertexAttribArray(positionAttrib);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

//        m_context.doneCurrent(); // why crash?
    }
}

void SceneWindow::resizeEvent(QResizeEvent *event)
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

void SceneWindow::mousePressEvent(QMouseEvent *event)
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

void SceneWindow::mouseMoveEvent(QMouseEvent *event)
{
    const QVector3D mouseImagePosition = m_transform.inverseMatrix().map(QVector3D(event->pos()));
    const QPoint lastPixel = QPoint(floor(lastMouseImagePos.x()), floor(lastMouseImagePos.y()));
    const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
    if (rect().contains(event->pos())) {
//        setFocus();
        if (m_scene && pixel != lastPixel && m_scene->imageData()->rect().contains(pixel)) {
            emit mousePixelChanged(pixel, m_scene->imageData()->pixel(pixel), m_scene->imageData()->format() == ImageDataFormat::Indexed ? m_scene->imageData()->pixel(pixel) : -1);
        }
    }
    else {
        emit mousePixelChanged(QPoint(0, 0), qRgba(0, 0, 0, 0), false);
    }
    if (!panKeyDown && event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        if (m_scene) {
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

void SceneWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));
    if (event->button() == Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        if (m_scene) {
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
        if (m_scene) {
            const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            Scene::ContextColour context = (event->modifiers() & Qt::SHIFT) ? Scene::Secondary : Scene::Primary;
            m_scene->pick(pixel, context);
        }
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void SceneWindow::wheelEvent(QWheelEvent *event)
{
    const bool transformAroundCursor = true;
    if (event->modifiers() & Qt::SHIFT) {
        Transform transform = m_transform;
        const qreal angle = event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        transform.setRotation(transform.rotation() + angle);
        if (transformAroundCursor) {
            const QVector3D mouseImagePosition = m_transform.inverseMatrix().map(QVector3D(event->pos()));
            const QVector3D mouseDelta = mouseImagePosition - transform.pan();
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

void SceneWindow::tabletEvent(QTabletEvent *event)
{
    if (false) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void SceneWindow::keyPressEvent(QKeyEvent *event)
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

void SceneWindow::keyReleaseEvent(QKeyEvent *event)
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

void SceneWindow::enterEvent(QEvent *const event)
{
    if (m_scene) {
        emit mouseEntered();
    }
}

void SceneWindow::leaveEvent(QEvent *const event)
{
    emit mouseLeft();
}

Scene *SceneWindow::image() const
{
    return m_scene;
}

Transform SceneWindow::transform() const
{
    return m_transform;
}

bool SceneWindow::tiled() const
{
    return m_tiled;
}

bool SceneWindow::showFrame() const
{
    return m_showFrame;
}

bool SceneWindow::showAlpha() const
{
    return m_showAlpha;
}

QRect SceneWindow::rect() const
{
    return QRect(0, 0, width(), height());
}

GLuint SceneWindow::vertexBuffer() const
{
    return m_vertexBuffer;
}

const QMatrix4x4 &SceneWindow::matrix()
{
    if (matricesDirty) {
        updateMatrices();
    }
    return m_matrix;
}

const QMatrix4x4 &SceneWindow::inverseMatrix()
{
    if (matricesDirty) {
        updateMatrices();
    }
    return m_inverseMatrix;
}

void SceneWindow::updateMatrices()
{
    m_matrix = QMatrix4x4();
    m_matrix.ortho(0., (float)width(), (float)height(), 0.f, -1.f, 1.f);
    m_inverseMatrix = m_matrix.inverted();
    matricesDirty = false;
}

void SceneWindow::setImage(Scene *const image)
{
    m_scene = image;
    Transform transform;
    transform.setOrigin(QVector3D((qreal)floor(width() / 2.f), floor((qreal)height() / 2.f), 0.f));
    transform.setPan(QVector3D(0.f, 0.f, 0.f));
    transform.setZoom(1.f);
    transform.setPixelAspect(QVector3D(1.f, 1.f, 0.f));
    transform.setRotation(0.f);
    if (image) {
//        setAttribute(Qt::WA_OpaquePaintEvent, true);
//        setAttribute(Qt::WA_NoSystemBackground, true);
        transform.setPan(-QVector3D(floor((qreal)m_scene->imageData()->size().width() / 2.f), floor((qreal)m_scene->imageData()->size().height() / 2.f), 0.f));
    }
    else {
//        setAttribute(Qt::WA_OpaquePaintEvent, false);
//        setAttribute(Qt::WA_NoSystemBackground, false);
    }
    setTransform(transform);
}

void SceneWindow::setTransform(const Transform &transform, const bool limit)
{
    if (m_transform != transform) {
        m_transform = transform;
        if (limit) {
            if (m_scene) {
                if (!m_tiled) {
                    m_transform.setPan(QVector3D(clamp(m_transform.pan().x(), (qreal)-m_scene->imageData()->size().width(), 0.f),
                                               clamp(m_transform.pan().y(), (qreal)-m_scene->imageData()->size().height(), 0.f),
                                               0.f));
//                    QPointF imageCentreScreenPosition = m_transform.matrix().map(QPointF((float)m_image->data().width() / 2.f, (float)m_image->data().height() / 2.f));
//                    QVector3D clampedImageCentreScreenPosition = QVector3D(clamp(imageCentreScreenPosition.x(), 0.f, (float)width()),
//                                                                         clamp(imageCentreScreenPosition.y(), 0.f, (float)height()),
//                                                                         0.f);
//                    m_transform.setPan(m_transform.inverseMatrix().map(clampedImageCentreScreenPosition));
                }
                else {
                    m_transform.setPan(QVector3D(wrap(m_transform.pan().x(), (qreal)-m_scene->imageData()->size().width(), 0.f),
                                               wrap(m_transform.pan().y(), (qreal)-m_scene->imageData()->size().height(), 0.f),
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

void SceneWindow::updateImage(const QRegion &region)
{
    if (!m_tiled && !region.isEmpty()) {
//        update(matrix.map(region)); // Too slow
//        update(m_transform.matrix().mapRect(region.boundingRect()));
        update();

    }
    else {
        update();
    }
}
void SceneWindow::setTiled(const bool tiled)
{
    if (m_tiled != tiled) {
        m_tiled = tiled;
        update();
        emit tiledChanged(tiled);
    }
}

void SceneWindow::setShowFrame(const bool showFrame)
{
    if (m_showFrame != showFrame) {
        m_showFrame = showFrame;
        update();
        emit showFrameChanged(showFrame);
    }
}

void SceneWindow::setShowAlpha(bool showAlpha)
{
    if (m_showAlpha != showAlpha) {
        m_showAlpha = showAlpha;
        update();
        emit showAlphaChanged(showAlpha);
    }
}

void SceneWindow::update() {
    renderLater();
}
