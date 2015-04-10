#include "imageeditor.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "application.h"
#include "util.h"
#include "transform.h"
#include <QOpenGLShaderProgram>
#include "imagedocument.h"

ImageEditor::ImageEditor(ImageDocument &_document, QWidget *parent) :
    QOpenGLWidget(parent), Editor(_document), m_transform(), m_tiled(false), m_tileX(true), m_tileY(true), panKeyDown(false), m_showAlpha(true), m_showBounds(false), m_vertexBuffer(0), m_editingContext(), m_limitTransform(true), viewToWorld(), worldToClip()
{
    ImageDocument &image = static_cast<ImageDocument &>(document);
    setMouseTracking(true);

    m_transform.setPan(QPointF(0.f, 0.f));
    m_transform.setZoom(1.f);
    m_transform.setPixelSize(QPointF(1.f, 1.f));
    m_transform.setRotation(0.f);
    m_transform.setPan(-QPointF(floor((float)image.imageData()->size().width() / 2.f), floor((float)image.imageData()->size().height() / 2.f)));
    updateTransform();
    m_editingContext.setImage(image.imageData());
    m_editingContext.setPalette(image.paletteData());
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
    glDeleteBuffers((GLsizei)1, &m_vertexBuffer);
    glGenBuffers((GLsizei)1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    const float halfWidth = (float)w / 2.f;
    const float halfHeight = (float)h / 2.f;
    const GLfloat vertices[][2] = {
        {-halfWidth, -halfHeight},
        {halfWidth, -halfHeight},
        {halfWidth, halfHeight},
        {-halfWidth, halfHeight},
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    worldToClip.setToIdentity();
    worldToClip.scale(1.f / halfWidth, -1.f / halfHeight);
    qDebug() << "worldToClip" << worldToClip.map(QPointF(-halfWidth, -halfHeight)) << worldToClip.map(QPointF(halfWidth, halfHeight));

    viewToWorld.setToIdentity();
    viewToWorld.translate(-halfWidth, -halfHeight);
    qDebug() << "viewToWorld" << viewToWorld.map(QPointF(0, 0)) << viewToWorld.map(QPointF(w, h));
}

void ImageEditor::paintGL()
{
    ImageDocument &image = static_cast<ImageDocument &>(document);
    if (!m_tiled || !m_tileX || !m_tileY || m_showAlpha) {
        GLuint program = APP->program("checkerboard");
        glUseProgram(program);

        GLint sizeUniform = glGetUniformLocation(program, "size");
        glUniform2f(sizeUniform, 32.f, 32.f);
        const QColor base = QColor(127, 127, 127);
        const QColor light = colourAdjustLightness(base, 16);
        GLint colour0Uniform = glGetUniformLocation(program, "colour0");
        glUniform4i(colour0Uniform, light.red(), light.green(), light.blue(), 255);
        const QColor dark = colourAdjustLightness(base, -16);
        GLint colour1Uniform = glGetUniformLocation(program, "colour1");
        glUniform4i(colour1Uniform, dark.red(), dark.green(), dark.blue(), 255);

        GLint matrixUniform = glGetUniformLocation(program, "matrix");
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, worldToClip.constData());

        GLint positionAttrib = glGetAttribLocation(program, "position");
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    QRect tilingBounds(0, 0, 1, 1);
    if (m_tiled) {
        QMatrix4x4 matrix = m_transform.inverseMatrix() * viewToWorld;
        QRectF bounds(matrix.map(QPointF(0.f, 0.f)), QSizeF(0.f, 0.f));
        expandRect(bounds, matrix.map(QPointF((float)width(), 0.f)));
        expandRect(bounds, matrix.map(QPointF(0.f, (float)height())));
        expandRect(bounds, matrix.map(QPointF((float)width(), (float)height())));
        if (m_tileX) {
            tilingBounds.setLeft((int)floor(bounds.left() / image.imageData()->size().width()));
            tilingBounds.setRight((int)floor(bounds.right() / image.imageData()->size().width()));
        }
        if (m_tileY) {
            tilingBounds.setTop((int)floor(bounds.top() / image.imageData()->size().height()));
            tilingBounds.setBottom((int)floor(bounds.bottom() / image.imageData()->size().height()));
        }
    }
//    qDebug() << tilingBounds;

    const int numberOfInstances = tilingBounds.width() * tilingBounds.height();

    if (m_showAlpha) {
        glEnable(GL_BLEND);
    }

    const int textureUnit = 0;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, image.imageData()->texture());

    const int palleteTextureUnit = 1;
    if (image.paletteData()) {
        glActiveTexture(GL_TEXTURE0 + palleteTextureUnit);
        glBindTexture(GL_TEXTURE_2D, image.paletteData()->texture());
    }

    GLuint program = APP->program("image");
    glUseProgram(program);

    GLint textureUnitUniform = glGetUniformLocation(program, "textureUnit");
    glUniform1i(textureUnitUniform, textureUnit);
    GLint isIndexedUniform = glGetUniformLocation(program, "isIndexed");
    glUniform1i(isIndexedUniform, (image.imageData()->format() == TextureDataFormat::Indexed));
    GLint hasPaletteUniform = glGetUniformLocation(program, "hasPalette");
    glUniform1i(hasPaletteUniform, (image.paletteData() != nullptr));
    GLint paletteTextureUnitUniform = glGetUniformLocation(program, "paletteTextureUnit");
    glUniform1i(paletteTextureUnitUniform, palleteTextureUnit);
    GLint tilesStartUniform = glGetUniformLocation(program, "tilesStart");
    glUniform2i(tilesStartUniform, tilingBounds.x(), tilingBounds.y());
    GLint tilesSizeUniform = glGetUniformLocation(program, "tilesSize");
    glUniform2i(tilesSizeUniform, tilingBounds.width(), tilingBounds.height());

    GLint matrixUniform = glGetUniformLocation(program, "matrix");
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, (worldToClip * m_transform.matrix()).constData());

    GLint positionAttrib = glGetAttribLocation(program, "position");
    glBindBuffer(GL_ARRAY_BUFFER, image.imageData()->vertexBuffer());
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, numberOfInstances);

    glDisableVertexAttribArray(positionAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);

    if (m_showAlpha) {
        glDisable(GL_BLEND);
    }

    if (m_showBounds) {
        GLuint program = APP->program("frame");
        glUseProgram(program);

        GLint colourUniform = glGetUniformLocation(program, "colour");
        glUniform4i(colourUniform, 255, 255, 255, 127);

        GLint matrixUniform = glGetUniformLocation(program, "matrix");
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, (worldToClip * m_transform.matrix()).constData());

        GLint positionAttrib = glGetAttribLocation(program, "position");
        glBindBuffer(GL_ARRAY_BUFFER, image.imageData()->vertexBuffer());
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        QRect tilingBounds(0, 0, 1, 1);
        GLint tilesStartUniform = glGetUniformLocation(program, "tilesStart");
        glUniform2i(tilesStartUniform, tilingBounds.x(), tilingBounds.y());
        GLint tilesSizeUniform = glGetUniformLocation(program, "tilesSize");
        glUniform2i(tilesSizeUniform, tilingBounds.width(), tilingBounds.height());

        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glDisableVertexAttribArray(positionAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUseProgram(0);
    }
}

void ImageEditor::mousePressEvent(QMouseEvent *event)
{
    QMatrix4x4 matrix = m_transform.inverseMatrix() * viewToWorld;
    mouseLastPos = event->pos();
    mouseLastImagePos = matrix.map(QPointF(event->pos()));
    if (event->button() == Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        QApplication::setOverrideCursor(Qt::CrossCursor);
        event->accept();
    }
    else if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        mouseGrabPos = event->pos();
        mouseGrabImagePos = matrix.map(QPointF(event->pos()));
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
    ImageDocument &image = static_cast<ImageDocument &>(document);
    QMatrix4x4 matrix = m_transform.inverseMatrix() * viewToWorld;
    const QPointF mouseImagePos = matrix.map(QPointF(event->pos()));
    const QPoint lastPixel = QPoint(floor(mouseLastImagePos.x()), floor(mouseLastImagePos.y()));
    const QPoint pixel = QPoint(floor(mouseImagePos.x()), floor(mouseImagePos.y()));
    if (rect().contains(event->pos())) {
        QColor colour = QColor();
        int index = -1;
        if (pixel != lastPixel && image.imageData()->rect().contains(pixel)) {
            ContextGrabber grab(APP->shareWidget());
            if (image.imageData()->format() == TextureDataFormat::Indexed) {
                index = image.imageData()->pixel(pixel);
                colour = QColor(image.paletteData()->colour(index));
            }
            else {
                colour = QColor(image.imageData()->pixel(pixel));
            }
        }
        emit mousePixelChanged(pixel, colour, index);
    }
    if (!panKeyDown && event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        image.stroke(lastPixel, pixel, &m_editingContext);
        mouseLastPos = event->pos();
        mouseLastImagePos = mouseImagePos;
        event->accept();
    }
    else if (event->buttons() & Qt::RightButton) {

    }
    else if (panKeyDown || event->buttons() & Qt::MiddleButton || (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        m_transform.setPan(m_transform.pan() - mouseGrabImagePos + mouseImagePos);
        updateTransform();
        event->accept();
    }
    else {
        event->ignore();
    }
    mouseLastPos = event->pos();
    mouseLastImagePos = mouseImagePos;
}

void ImageEditor::mouseReleaseEvent(QMouseEvent *event)
{
    ImageDocument &image = static_cast<ImageDocument &>(document);
    QMatrix4x4 matrix = m_transform.inverseMatrix() * viewToWorld;
    QPointF mouseImagePos = matrix.map(QPointF(event->pos()));
    if (event->button() == Qt::LeftButton && !(event->modifiers() & Qt::CTRL)) {
        const QPoint pixel = QPoint(floor(mouseImagePos.x()), floor(mouseImagePos.y()));
        image.point(pixel, &m_editingContext);
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)) {
        QApplication::restoreOverrideCursor();
        event->accept();
    }
    else if (event->button() == Qt::RightButton) {
        const QPoint pixel = QPoint(floor(mouseImagePos.x()), floor(mouseImagePos.y()));
//            Scene::ContextColour context = (event->modifiers() & Qt::SHIFT) ? Scene::Secondary : Scene::Primary;
        ContextGrabber grab(APP->shareWidget());
        image.pick(pixel, &editingContext());
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
        const float angle = event->angleDelta().y() > 0 ? 15 : (event->angleDelta().y() < 0 ? -15 : 0);
        m_transform.setRotation(m_transform.rotation() + angle);
        if (transformAroundCursor) {
            const QPointF mouseImagePos = m_transform.inverseMatrix().map(QPointF(event->pos()));
            const QPointF mouseDelta = mouseImagePos - m_transform.pan();
        }
        updateTransform();
        event->accept();
    }
    else {
        auto zoomToNearestIndex = [](const float zoom, const bool halfSteps = true, const float zoomStep = 2) {
            const float exponent = round(log(zoom) / log(zoomStep));
            const float halfStep = pow(zoomStep, exponent - 1);
            const float remainder = zoom - zoomStep * halfStep;
            const float rounding = round(remainder / halfStep);
            return (int)(exponent * 2 + rounding);
        };
        int index = zoomToNearestIndex(m_transform.zoom()) + (event->angleDelta().y() > 0 ? 1 : 0) + (event->angleDelta().y() < 0 ? -1 : 0);

        auto indexToZoom = [](const int index, const bool halfSteps = true, const float zoomStep = 2) {
            const float exponent = floor((float)index / (float)2);
            const float halfStep = pow(zoomStep, exponent - 1);
            return zoomStep * halfStep + (index - (2 * exponent) == 1 ? halfStep : 0);
        };
        m_transform.setZoom(indexToZoom(index));

        if (transformAroundCursor) {
            const QPointF mouseImagePos = m_transform.inverseMatrix().map(QPointF(event->pos()));

        }
        updateTransform();
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
        mouseLastPos = mapFromGlobal(QCursor::pos());
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

void ImageEditor::applyTransformLimits()
{
    ImageDocument &image = static_cast<ImageDocument &>(document);
    float panX = (m_tiled && m_tileX) ? wrap((float)m_transform.pan().x(), (float)-image.imageData()->size().width(), 0.f) : clamp((float)m_transform.pan().x(), (float)-image.imageData()->size().width(), 0.f);
    float panY = (m_tiled && m_tileY) ? wrap((float)m_transform.pan().y(), (float)-image.imageData()->size().height(), 0.f) : clamp((float)m_transform.pan().y(), (float)-image.imageData()->size().height(), 0.f);
    m_transform.setPan(QPointF(panX, panY));
    m_transform.setZoom(clamp(m_transform.zoom(), 1.f/16.f, 256.f));
    m_transform.setPixelSize(QPointF(clamp((float)m_transform.pixelSize().x(), 1.f/16.f, 16.f), clamp((float)m_transform.pixelSize().y(), 1.f/16.f, 16.f)));
    m_transform.setRotation(wrap(m_transform.rotation(), 0.f, 360.f));
}

void ImageEditor::updateTransform()
{
    applyTransformLimits();
    update();
}
