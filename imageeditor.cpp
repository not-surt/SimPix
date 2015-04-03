#include "imageeditor.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "application.h"
#include "util.h"
#include "transform.h"
#include <QOpenGLShaderProgram>

ImageEditor::ImageEditor(QWidget *parent) :
    QOpenGLWidget(parent), m_image(nullptr), m_transform(), m_tiled(false), panKeyDown(false), m_showFrame(false), matricesDirty(true), m_vertexBuffer(0), m_editingContext(), m_limitTransform(true)
{
    setImage(nullptr);
    setMouseTracking(true);
}

ImageEditor::~ImageEditor()
{

}

void ImageEditor::initializeGL()
{

}

void ImageEditor::resizeGL(int w, int h)
{
    initializeOpenGLFunctions();
    glViewport(0, 0, (GLint)w, (GLint)h);
    glDeleteBuffers((GLsizei)1, &m_vertexBuffer);
    glGenBuffers((GLsizei)1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    const GLfloat vertices[][3] = {
        {0.f, 0.f, 0.f},
        {(GLfloat)w, 0.f, 0.f},
        {(GLfloat)w, (GLfloat)h, 0.f},
        {0.f, (GLfloat)h, 0.f},
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    m_transform.setOrigin(QVector3D(floor(w / 2.f), floor(h / 2.f), 0.f));
    matricesDirty = true;
}

void ImageEditor::paintGL()
{
    initializeOpenGLFunctions();
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    if (true || !m_tiled || m_showAlpha) {
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

        const QColor base = QColor(127, 127, 127), light = colourAdjustLightness(base, 16), dark = colourAdjustLightness(base, -16);
        glUniform4i(colour0Uniform, light.red(), light.green(), light.blue(), 255);
        glUniform4i(colour1Uniform, dark.red(), dark.green(), dark.blue(), 255);
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
            glDisable(GL_BLEND);
            GLuint program = APP->program("frame");
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
    }
}

void ImageEditor::mousePressEvent(QMouseEvent *event)
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
    update();
}

void ImageEditor::mouseMoveEvent(QMouseEvent *event)
{
    const QVector3D mouseImagePosition = m_transform.inverseMatrix().map(QVector3D(event->pos()));
    const QPoint lastPixel = QPoint(floor(lastMouseImagePos.x()), floor(lastMouseImagePos.y()));
    const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
    QPoint coord = QPoint(0, 0);
    QColor colour = QColor();
    int index = -1;
    if (rect().contains(event->pos())) {
//        setFocus();
        coord = pixel;
        if (m_image && pixel != lastPixel && m_image->imageData()->rect().contains(pixel)) {
            ContextGrabber grab(APP->shareWidget());
//            ContextGrabber grab();
            if (m_image->imageData()->format() == ImageDataFormat::Indexed) {
                index = m_image->imageData()->pixel(pixel);
                colour = QColor(m_image->paletteData()->colour(index));
            }
            else {
                colour = QColor(m_image->imageData()->pixel(pixel));
            }
        }
        emit mousePixelChanged(coord, colour, index);
    }
    if (!panKeyDown && event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        if (m_image) {
            emit dragged(lastPixel, pixel, &m_editingContext);
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
    update();
}

void ImageEditor::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));
    if (event->button() == Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        if (m_image) {
            const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
            emit clicked(pixel, &m_editingContext);
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
//            Scene::ContextColour context = (event->modifiers() & Qt::SHIFT) ? Scene::Secondary : Scene::Primary;
            ContextGrabber grab(APP->shareWidget());
            m_image->pick(pixel, &editingContext());
        }
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else {
        event->ignore();
    }
    update();
}

void ImageEditor::wheelEvent(QWheelEvent *event)
{
    const bool transformAroundCursor = true;
    if (event->modifiers() & Qt::SHIFT) {
        Transform transform = m_transform;
        const float angle = event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
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
        const float scale = event->angleDelta().y() > 0 ? 2 : (event->angleDelta().y() < 0 ? .5 : 1);
        transform.setZoom(transform.zoom() * scale);
        if (transformAroundCursor) {
            const QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));

        }
        setTransform(transform);
        event->accept();
    }
}

void ImageEditor::tabletEvent(QTabletEvent *event)
{
    if (false) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void ImageEditor::keyPressEvent(QKeyEvent *event)
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

void ImageEditor::keyReleaseEvent(QKeyEvent *event)
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

void ImageEditor::enterEvent(QEvent *const event)
{
    if (m_image) {
        emit mouseEntered();
    }
}

void ImageEditor::leaveEvent(QEvent *const event)
{
    emit mouseLeft();
}

Image *ImageEditor::image() const
{
    return m_image;
}

const Transform &ImageEditor::transform() const
{
    return m_transform;
}

bool ImageEditor::tiled() const
{
    return m_tiled;
}

bool ImageEditor::showFrame() const
{
    return m_showFrame;
}

bool ImageEditor::showAlpha() const
{
    return m_showAlpha;
}

QRect ImageEditor::rect() const
{
    return QRect(0, 0, width(), height());
}

GLuint ImageEditor::vertexBuffer() const
{
    return m_vertexBuffer;
}

const QMatrix4x4 &ImageEditor::matrix()
{
    if (matricesDirty) {
        updateMatrices();
    }
    return m_matrix;
}

const QMatrix4x4 &ImageEditor::inverseMatrix()
{
    if (matricesDirty) {
        updateMatrices();
    }
    return m_inverseMatrix;
}

void ImageEditor::updateMatrices()
{
    m_matrix = QMatrix4x4();
    m_matrix.ortho(0., (float)width(), (float)height(), 0.f, -1.f, 1.f);
    m_inverseMatrix = m_matrix.inverted();
    matricesDirty = false;
}

EditingContext &ImageEditor::editingContext()
{
    return m_editingContext;
}

bool ImageEditor::limitTransform() const
{
    return m_limitTransform;
}

void ImageEditor::setImage(Image *const image)
{
    m_image = image;
    Transform transform;
    transform.setOrigin(QVector3D((float)floor(width() / 2.f), floor((float)height() / 2.f), 0.f));
    transform.setPan(QVector3D(0.f, 0.f, 0.f));
    transform.setZoom(1.f);
    transform.setPixelAspect(QVector3D(1.f, 1.f, 0.f));
    transform.setRotation(0.f);
    if (image) {
        transform.setPan(-QVector3D(floor((float)m_image->imageData()->size().width() / 2.f), floor((float)m_image->imageData()->size().height() / 2.f), 0.f));
        m_editingContext.setImage(m_image->imageData());
        m_editingContext.setPalette(m_image->paletteData());
    }
    else {
        m_editingContext.setImage(nullptr);
        m_editingContext.setPalette(nullptr);
    }
    setTransform(transform);
}

void ImageEditor::setTransform(const Transform &transform)
{
    if (m_transform != transform) {
        m_transform = transform;
        if (m_limitTransform) {
            if (m_image) {
                if (!m_tiled) {
                    m_transform.setPan(QVector3D(clamp(m_transform.pan().x(), (float)-m_image->imageData()->size().width(), 0.f),
                                               clamp(m_transform.pan().y(), (float)-m_image->imageData()->size().height(), 0.f),
                                               0.f));
                }
                else {
                    m_transform.setPan(QVector3D(wrap(m_transform.pan().x(), (float)-m_image->imageData()->size().width(), 0.f),
                                               wrap(m_transform.pan().y(), (float)-m_image->imageData()->size().height(), 0.f),
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

void ImageEditor::setTiled(const bool tiled)
{
    if (m_tiled != tiled) {
        m_tiled = tiled;
        update();
        emit tiledChanged(tiled);
    }
}

void ImageEditor::setShowFrame(const bool showFrame)
{
    if (m_showFrame != showFrame) {
        m_showFrame = showFrame;
        update();
        emit showFrameChanged(showFrame);
    }
}

void ImageEditor::setShowAlpha(bool showAlpha)
{
    if (m_showAlpha != showAlpha) {
        m_showAlpha = showAlpha;
        update();
        emit showAlphaChanged(showAlpha);
    }
}

void ImageEditor::setLimitTransform(bool arg)
{
    if (m_limitTransform != arg) {
        m_limitTransform = arg;
        emit limitTransformChanged(arg);
    }
}
