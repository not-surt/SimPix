#include "imageeditor.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "application.h"
#include "util.h"
#include "transform.h"
#include <QOpenGLShaderProgram>

ImageEditor::ImageEditor(Image *image, QWidget *parent) :
    QOpenGLWidget(parent), m_image(image), m_transform(), m_tiled(false), m_tileX(true), m_tileY(true), panKeyDown(false), m_showBounds(false), matricesDirty(true), m_vertexBuffer(0), m_editingContext(), m_limitTransform(true)
{
    setMouseTracking(true);

    Transform transform;
    transform.setOrigin(QVector3D((float)floor(width() / 2.f), floor((float)height() / 2.f), 0.f));
    transform.setPan(QVector3D(0.f, 0.f, 0.f));
    transform.setZoom(1.f);
    transform.setPixelAspect(QVector3D(1.f, 1.f, 0.f));
    transform.setRotation(0.f);
    transform.setPan(-QVector3D(floor((float)m_image->imageData()->size().width() / 2.f), floor((float)m_image->imageData()->size().height() / 2.f), 0.f));
    setTransform(transform);
    m_editingContext.setImage(m_image->imageData());
    m_editingContext.setPalette(m_image->paletteData());
}

ImageEditor::~ImageEditor()
{

}

void ImageEditor::initializeGL()
{
    initializeOpenGLFunctions();

    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
}

void ImageEditor::resizeGL(int w, int h)
{
    initializeOpenGLFunctions();

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

    if (!m_tiled || !m_tileX || !m_tileY || m_showAlpha) {
        GLuint program = APP->program("checkerboard");
        glUseProgram(program);

        GLint positionAttrib = glGetAttribLocation(program, "position");
        GLint matrixUniform = glGetUniformLocation(program, "matrix");
        GLint sizeUniform = glGetUniformLocation(program, "size");
        GLint offsetUniform = glGetUniformLocation(program, "offset");
        GLint colour0Uniform = glGetUniformLocation(program, "colour0");
        GLint colour1Uniform = glGetUniformLocation(program, "colour1");

        glUniform2f(sizeUniform, 32.f, 32.f);
        glUniform2f(offsetUniform, 0.f, 0.f);
        const QColor base = QColor(127, 127, 127), light = colourAdjustLightness(base, 16), dark = colourAdjustLightness(base, -16);
        glUniform4i(colour0Uniform, light.red(), light.green(), light.blue(), 255);
        glUniform4i(colour1Uniform, dark.red(), dark.green(), dark.blue(), 255);
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, this->matrix().constData());
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    QRect tilingBounds(0, 0, 1, 1);
    if (m_tiled) {
        QRectF bounds(m_transform.inverseMatrix().map(QPointF(0., 0.)), QSizeF(1., 1.));
        expandRect(bounds, m_transform.inverseMatrix().map(QPointF((float)width(), 0.)));
        expandRect(bounds, m_transform.inverseMatrix().map(QPointF(0., (float)height())));
        expandRect(bounds, m_transform.inverseMatrix().map(QPointF((float)width(), (float)height())));
        if (m_tileX) {
            tilingBounds.setLeft((int)floor(bounds.left() / m_image->imageData()->size().width()));
            tilingBounds.setRight((int)floor(bounds.right() / m_image->imageData()->size().width()));
        }
        if (m_tileY) {
            tilingBounds.setTop((int)floor(bounds.top() / m_image->imageData()->size().height()));
            tilingBounds.setBottom((int)floor(bounds.bottom() / m_image->imageData()->size().height()));
        }
        qDebug() << pos() << size() << bounds << tilingBounds;///////////////////////////////
    }

    for (int y = tilingBounds.top(); y <= tilingBounds.bottom(); y++) {
        for (int x = tilingBounds.left(); x <= tilingBounds.right(); x++) {
            float tileOffsetX = x * m_image->imageData()->size().width();
            float tileOffsetY = y * m_image->imageData()->size().height();

            if (m_showAlpha) {
                glEnable(GL_BLEND);
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
            GLint offsetUniform = glGetUniformLocation(program, "offset");
            GLint textureUnitUniform = glGetUniformLocation(program, "textureUnit");
            GLint isIndexedUniform = glGetUniformLocation(program, "isIndexed");
            GLint hasPaletteUniform = glGetUniformLocation(program, "hasPalette");
            GLint paletteTextureUnitUniform = glGetUniformLocation(program, "paletteTextureUnit");
            GLint matrixUniform = glGetUniformLocation(program, "matrix");

            glUniform1i(textureUnitUniform, 0);
            glUniform1i(isIndexedUniform, (m_image->imageData()->format() == ImageDataFormat::Indexed));
            glUniform1i(hasPaletteUniform, (m_image->paletteData() != nullptr));
            glUniform1i(paletteTextureUnitUniform, 1);
            glUniform2f(offsetUniform, tileOffsetX, tileOffsetY);

            QMatrix4x4 matrix;
            GLint vertexBuffer;
            matrix = this->matrix() * m_transform.matrix();
            vertexBuffer = m_image->imageData()->vertexBuffer();
            glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, matrix.constData());

            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glDisableVertexAttribArray(positionAttrib);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glUseProgram(0);

            if (m_showAlpha) {
                glDisable(GL_BLEND);
            }
        }
    }

    if (m_showBounds) {
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

        glUseProgram(0);
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
        if (pixel != lastPixel && m_image->imageData()->rect().contains(pixel)) {
            ContextGrabber grab(APP->shareWidget());
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
//            emit dragged(lastPixel, pixel, &m_editingContext);
        m_image->stroke(lastPixel, pixel, &m_editingContext);
        lastMousePos = event->pos();
        lastMouseImagePos = mouseImagePosition;
        event->accept();
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

void ImageEditor::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF mouseImagePosition = m_transform.inverseMatrix().map(QPointF(event->pos()));
    if (event->button() == Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
//            emit clicked(pixel, &m_editingContext);
        m_image->point(pixel, &m_editingContext);
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else if (event->button() == Qt::RightButton) {
        const QPoint pixel = QPoint(floor(mouseImagePosition.x()), floor(mouseImagePosition.y()));
//            Scene::ContextColour context = (event->modifiers() & Qt::SHIFT) ? Scene::Secondary : Scene::Primary;
        ContextGrabber grab(APP->shareWidget());
        m_image->pick(pixel, &editingContext());
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else {
        event->ignore();
    }
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
    emit mouseEntered();
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

bool ImageEditor::tileX() const
{
    return m_tileX;
}

bool ImageEditor::tileY() const
{
    return m_tileY;
}

bool ImageEditor::showBounds() const
{
    return m_showBounds;
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

void ImageEditor::setTransform(const Transform &transform)
{
    if (m_transform != transform) {
        m_transform = transform;
        if (m_limitTransform) {
            float panX = (m_tiled && m_tileX) ? wrap(m_transform.pan().x(), (float)-m_image->imageData()->size().width(), 0.f) : clamp(m_transform.pan().x(), (float)-m_image->imageData()->size().width(), 0.f);
            float panY = (m_tiled && m_tileY) ? wrap(m_transform.pan().y(), (float)-m_image->imageData()->size().height(), 0.f) : clamp(m_transform.pan().y(), (float)-m_image->imageData()->size().height(), 0.f);
            m_transform.setPan(QVector3D(panX, panY, 0.f));
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

void ImageEditor::setTileX(const bool tileX)
{
    if (m_tileX != tileX) {
        m_tileX = tileX;
        update();
        emit tiledChanged(tileX);
    }
}

void ImageEditor::setTileY(const bool tileY)
{
    if (m_tileY != tileY) {
        m_tileY = tileY;
        update();
        emit tiledChanged(tileY);
    }
}

void ImageEditor::setShowBounds(const bool showBounds)
{
    if (m_showBounds != showBounds) {
        m_showBounds = showBounds;
        update();
        emit showBoundsChanged(showBounds);
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
