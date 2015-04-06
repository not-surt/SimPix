#include "imageeditor.h"

#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "application.h"
#include "util.h"
#include "transform.h"
#include <QOpenGLShaderProgram>

ImageEditor::ImageEditor(Image *image, QWidget *parent) :
    QOpenGLWidget(parent), Editor(image), m_image(image), m_transform(), m_tiled(false), m_tileX(true), m_tileY(true), panKeyDown(false), m_showAlpha(true), m_showBounds(false), m_vertexBuffer(0), m_editingContext(), m_limitTransform(true), viewToClip(), clipToView()
{
    setMouseTracking(true);

    Transform transform;
    transform.setPan(QPointF(0.f, 0.f));
    transform.setZoom(1.f);
    transform.setPixelSize(QPointF(1.f, 1.f));
    transform.setRotation(0.f);
//    transform.setPan(-QPointF(floor((float)m_image->imageData()->size().width() / 2.f), floor((float)m_image->imageData()->size().height() / 2.f), 0.f));
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
    const float halfWidth = (float)w / 2.f;
    const float halfHeight = (float)h / 2.f;
    const GLfloat vertices[][2] = {
        {-halfWidth, -halfHeight},
        {halfWidth, -halfHeight},
        {halfWidth, halfHeight},
        {-halfWidth, halfHeight},
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    viewToClip.setToIdentity();
//    m_viewToClip.ortho(-halfWidth, halfWidth, halfHeight, -halfHeight, -1.f, 1.f);
    viewToClip.translate(-1.f, 1.f, 0.f);
    viewToClip.scale(2.f / (float)w, -2.f / (float)h, 1.f);
    clipToView = viewToClip.inverted();
//    // 0,0 -> -1,1
//    // w,h -> 1,-1
//    qDebug() << QPointF(0, 0) << viewToClip.map(QPointF(0, 0));
//    qDebug() << QPointF(w, h) << viewToClip.map(QPointF(w, h));
////     view -> clip -> world
////     world -> clip
    // view -> camera/clip -> world
    // world -> camera/clip
    // 0,0 -> -1,1 -> -w/2,-h/2
    // w/2,h/2 -> 0,0 -> 0,0
    // w,h -> 1,-1 -> w/2,h/2

    {
        const float w = 640, h = 480;
        const float halfWidth = (float)w / 2.f;
        const float halfHeight = (float)h / 2.f;

        QMatrix4x4 viewToClip;
        viewToClip.translate(-1.f, 1.f, 0.f);
        viewToClip.scale(2.f / (float)w, -2.f / (float)h, 1.f);
        qDebug() << viewToClip.map(QVector4D(0, 0, 0, 1)) << viewToClip.map(QVector4D(w, h, 0, 1));

        QMatrix4x4 clipToWorld;
        clipToWorld.scale((float)w / 2.f, (float)h / -2.f, 1.f);
        qDebug() << clipToWorld.map(QVector4D(-1, 1, 0, 1)) << clipToWorld.map(QVector4D(1, -1, 0, 1));

        QMatrix4x4 viewToWorld;
        viewToWorld = viewToClip * clipToWorld;
        qDebug() << viewToWorld.map(QVector4D(0, 0, 0, 1)) << viewToWorld.map(QVector4D(w, h, 0, 1));
    }
}

void ImageEditor::paintGL()
{
    initializeOpenGLFunctions();

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
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, viewToClip.constData());

        GLint positionAttrib = glGetAttribLocation(program, "position");
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    QRect tilingBounds(0, 0, 1, 1);
//    if (m_tiled) {
//        QRectF bounds(m_transform.clipToWorld().map(QPointF(0., 0.)), QSizeF(1., 1.));
//        expandRect(bounds, m_transform.clipToWorld().map(QPointF((float)width(), 0.)));
//        expandRect(bounds, m_transform.clipToWorld().map(QPointF(0., (float)height())));
//        expandRect(bounds, m_transform.clipToWorld().map(QPointF((float)width(), (float)height())));
//        if (m_tileX) {
//            tilingBounds.setLeft((int)floor(bounds.left() / m_image->imageData()->size().width()));
//            tilingBounds.setRight((int)floor(bounds.right() / m_image->imageData()->size().width()));
//        }
//        if (m_tileY) {
//            tilingBounds.setTop((int)floor(bounds.top() / m_image->imageData()->size().height()));
//            tilingBounds.setBottom((int)floor(bounds.bottom() / m_image->imageData()->size().height()));
//        }
//    }
//    qDebug() << tilingBounds;

    glClear(GL_COLOR_BUFFER_BIT);///////////////////////////////////////////////

    const int numberOfInstances = tilingBounds.width() * tilingBounds.height();

    if (m_showAlpha) {
        glEnable(GL_BLEND);
    }

    const int textureUnit = 0;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_image->imageData()->texture());

    const int palleteTextureUnit = 1;
    if (m_image->paletteData()) {
        glActiveTexture(GL_TEXTURE0 + palleteTextureUnit);
        glBindTexture(GL_TEXTURE_2D, m_image->paletteData()->texture());
    }

    GLuint program = APP->program("image");
    glUseProgram(program);

    GLint textureUnitUniform = glGetUniformLocation(program, "textureUnit");
    glUniform1i(textureUnitUniform, textureUnit);
    GLint isIndexedUniform = glGetUniformLocation(program, "isIndexed");
    glUniform1i(isIndexedUniform, (m_image->imageData()->format() == ImageDataFormat::Indexed));
    GLint hasPaletteUniform = glGetUniformLocation(program, "hasPalette");
    glUniform1i(hasPaletteUniform, (m_image->paletteData() != nullptr));
    GLint paletteTextureUnitUniform = glGetUniformLocation(program, "paletteTextureUnit");
    glUniform1i(paletteTextureUnitUniform, palleteTextureUnit);
    GLint tilesStartUniform = glGetUniformLocation(program, "tilesStart");
    glUniform2i(tilesStartUniform, tilingBounds.x(), tilingBounds.y());
    GLint tilesSizeUniform = glGetUniformLocation(program, "tilesSize");
    glUniform2i(tilesSizeUniform, tilingBounds.width(), tilingBounds.height());

    GLint matrixUniform = glGetUniformLocation(program, "matrix");
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, (viewToClip * m_transform.worldToClip()).constData());
//    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, m_transform.worldToClip().constData());

    GLint positionAttrib = glGetAttribLocation(program, "position");
    glBindBuffer(GL_ARRAY_BUFFER, m_image->imageData()->vertexBuffer());
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
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, (viewToClip * m_transform.worldToClip()).constData());

        GLint positionAttrib = glGetAttribLocation(program, "position");
        glBindBuffer(GL_ARRAY_BUFFER, m_image->imageData()->vertexBuffer());
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glDisableVertexAttribArray(positionAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUseProgram(0);
    }
}

void ImageEditor::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
    lastMouseImagePos = m_transform.clipToWorld().map(QPointF(event->pos()));
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
//    qDebug() << viewToClip.map(QPointF(event->pos()));/////////////////////////////////////////////////////////
    const QPointF mouseImagePosition = m_transform.clipToWorld().map(QPointF(event->pos()));
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
        const QPointF delta = mouseImagePosition - m_transform.clipToWorld().map(QPointF(lastMousePos));
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
    QPointF mouseImagePosition = m_transform.clipToWorld().map(QPointF(event->pos()));
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
            const QPointF mouseImagePosition = m_transform.clipToWorld().map(QPointF(event->pos()));
            const QPointF mouseDelta = mouseImagePosition - transform.pan();
        }
        setTransform(transform);
        event->accept();
    }
    else {
        Transform transform = m_transform;
        const float scale = event->angleDelta().y() > 0 ? 2 : (event->angleDelta().y() < 0 ? .5 : 1);
        transform.setZoom(transform.zoom() * scale);
        if (transformAroundCursor) {
            const QPointF mouseImagePosition = m_transform.clipToWorld().map(QPointF(event->pos()));

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

void ImageEditor::setTransform(const Transform &transform)
{
    if (m_transform != transform) {
        m_transform = transform;
        if (m_limitTransform) {
            float panX = (m_tiled && m_tileX) ? wrap((float)m_transform.pan().x(), (float)-m_image->imageData()->size().width(), 0.f) : clamp((float)m_transform.pan().x(), (float)-m_image->imageData()->size().width(), 0.f);
            float panY = (m_tiled && m_tileY) ? wrap((float)m_transform.pan().y(), (float)-m_image->imageData()->size().height(), 0.f) : clamp((float)m_transform.pan().y(), (float)-m_image->imageData()->size().height(), 0.f);
            m_transform.setPan(QPointF(panX, panY));
            m_transform.setZoom(clamp(m_transform.zoom(), 1.f/16.f, 256.f));
            m_transform.setPixelSize(QPointF(clamp((float)m_transform.pixelSize().x(), 1.f/16.f, 16.f), clamp((float)m_transform.pixelSize().y(), 1.f/16.f, 16.f)));
            m_transform.setRotation(wrap(m_transform.rotation(), 0.f, 360.f));
        }
        update();
        emit transformChanged(m_transform);
    }
}
