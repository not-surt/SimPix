#include "document.h"

#include "util.h"

Image::Image(const QSize &size, ImageDataFormat format, QObject *parent) :
    Document(parent), m_imageData(nullptr), m_paletteData(nullptr)
{
    ContextGrabber grab(APP->shareWidget());

    if (format == ImageDataFormat::Invalid) {
        qDebug() << "Invalid image format";
        return;
    }

    uint fillColour = 0;
    if (format == ImageDataFormat::Indexed) {
        m_paletteData = new PaletteData(APP->shareWidget(), 2);
        m_paletteData->setColour(0, qRgba(0, 0, 0, 255));
        m_paletteData->setColour(1, qRgba(255, 255, 255, 255));
        fillColour = 1;
//        m_contextColours[ContextColour::Primary] = 1;
//        m_contextColours[ContextColour::Secondary] = m_contextColours[ContextColour::Eraser] = 0;
    }
    else if (format == ImageDataFormat::RGBA) {
        fillColour = qRgba(255, 0, 0, 255);
//        m_contextColours[ContextColour::Primary] = qRgba(255, 255, 255, 255);
//        m_contextColours[ContextColour::Secondary] = m_contextColours[ContextColour::Eraser] = qRgba(0, 0, 0, 0);
    }

    m_imageData = new ImageData(APP->shareWidget(), size, format);
    m_imageData->clear(fillColour);
}

Image::Image(const QString &fileName, const char *fileFormat, QObject *parent) :
    Document(fileName, parent)
{
    ContextGrabber grab(APP->shareWidget());

    QImage image(m_fileName, fileFormat);
    if (!image.isNull()) {
        switch (image.format()) {
        case QImage::Format_Invalid:
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
        format = ImageDataFormat::Invalid;
        qDebug() << "Invalid image format";
        return;
    }

    m_paletteData = nullptr;
    if (!image.colorTable().isEmpty()) {
        m_paletteData = new PaletteData(APP->shareWidget(), image.colorTable().size(), (GLubyte *)image.colorTable().constData());
    }

    switch (format) {
    case ImageDataFormat::Indexed:
        if (m_paletteData && m_paletteData->length() > 0) {
//            m_contextColours[ContextColour::Primary] = m_paletteData->colour(m_paletteData->length() - 1);
        }
        else {
//            m_contextColours[ContextColour::Primary] = 255;
        }
//        m_contextColours[ContextColour::Secondary] = 0;
        break;
    case ImageDataFormat::RGBA:
//        m_contextColours[ContextColour::Primary] = qRgba(255, 255, 255, 255);
//        m_contextColours[ContextColour::Secondary] = qRgba(0, 0, 0, 0);
        break;
    }

    m_imageData = new ImageData(APP->shareWidget(), image.size(), format, image.constBits());
}

Image::~Image()
{
    ContextGrabber grab(APP->shareWidget());
    delete m_imageData;
    delete m_paletteData;
}

Editor *Image::createEditor()
{
    return nullptr;
}

Document::Document(QObject *parent) :
    QObject(parent), m_fileName(QString()), m_dirty(false)
{

}

Document::Document(const QString &fileName, QObject *parent) :
    QObject(parent), m_fileName(fileName), m_dirty(false)
{

}

bool Document::revert()
{
    return doOpen(m_fileName);
}

void Document::makeDirty()
{
    m_dirty = true;
    emit dirtied();
    emit dirtyChanged();
}

bool Document::save(QString fileName)
{
    bool saved = false;
    if (fileName.isNull()) {
        fileName = m_fileName;
    }
    if (!fileName.isNull()) {
        saved = doSave(fileName);
    }
    return saved;
}

bool Image::doOpen(QString fileName)
{
    return true;
}

bool Image::doSave(QString fileName)
{

    bool saved = false;

    APP->shareWidget()->makeCurrent();
    uchar *data = m_imageData->readData();
    QImage::Format format;
    switch (m_imageData->format()) {
    case ImageDataFormat::Indexed:
        format = QImage::Format_Indexed8;
        break;
    case ImageDataFormat::RGBA:
        format = QImage::Format_ARGB32;
        break;
    default:
        delete data;
        return false;
    }
    QImage qImage = QImage(data, m_imageData->size().width(), m_imageData->size().height(), format);
    if (m_paletteData && (format == QImage::Format_Indexed8)) {
        uchar *palette = m_paletteData->readData();
        std::vector<QRgb> vector((QRgb *)palette, ((QRgb *)palette) + m_paletteData->length());
        qImage.setColorTable(QVector<QRgb>::fromStdVector(vector));
        delete palette;
    }
    saved = qImage.save(fileName);
    if (saved) {
        m_dirty = false;
        m_fileName = fileName;
    }
    delete data;

    return saved;
}

void drawPixel(TextureData &texture, const QPoint &point, const uint colour, const void *const data = nullptr)
{

    if (QRect(QPoint(0, 0), texture.size()).contains(point)) {
        texture.setPixel(point, colour);
    }

    QOpenGLFunctions gl;
    gl.initializeOpenGLFunctions();

    gl.glBindFramebuffer(GL_FRAMEBUFFER, texture.framebuffer());

    GLint program = 0;
    gl.glUseProgram(program);

    GLuint vertexBuffer;
    gl.glGenBuffers((GLsizei)1, &vertexBuffer);
    gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    const GLfloat vertices[][2] = {
        {-1.f, -1.f},
        {1.f, -1.f},
        {1.f, 1.f},
        {-1.f, 1.f},
    };
    gl.glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//    GLint positionAttrib = gl.glGetAttribLocation(program, "position");
//    gl.glEnableVertexAttribArray(positionAttrib);
//    gl.glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
//    gl.glDisableVertexAttribArray(positionAttrib);
//    gl.glBindBuffer(GL_ARRAY_BUFFER, 0);

//    gl.glDisableVertexAttribArray(positionAttrib);
    gl.glBindBuffer(GL_ARRAY_BUFFER, 0);

    gl.glUseProgram(0);
    gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawRectangularBrush(QImage &image, const QPoint &point, const uint colour, const void *const data)
{
    const uint *const size = (const uint *)data;
}

void drawEllipticalBrush(TextureData &texture, const QPoint &point, const uint colour, const void *const data)
{
    const uint *const size = (const uint *)data;
}

void drawAngularBrush(TextureData &texture, const QPoint &point, const uint colour, const void *const data)
{
    const int *const size = (const int *)data;
}

void doLine(TextureData &texture, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(TextureData &texture, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
{
    QPoint delta = point1 - point0;
    const int stepX = sign(delta.x()), stepY = sign(delta.y());
    int sumStepX = abs(delta.y()), sumStepY = abs(delta.x());
    int x = point0.x(), y = point0.y();
    if (sumStepX > 0 || sumStepY > 0) {
        if (sumStepX == 0) {
            sumStepX = sumStepY;
        }
        if (sumStepY == 0) {
            sumStepY = sumStepX;
        }
        const int limit = sumStepX * sumStepY;
        int sumX = sumStepX, sumY = sumStepY;
        do {
            callback(texture, QPoint(x, y), colour, data);
            if (sumX >= sumY) {
                y += stepY;
                sumY += sumStepY;
            }
            if (sumX <= sumY) {
                x += stepX;
                sumX += sumStepX;
            }
        } while (sumX <= limit && sumY <= limit);
    }
    if (inclusive) {
        callback(texture, QPoint(x, y), colour, data);
    }
}

void drawLine(TextureData &texture, const QPoint &point0, const QPoint &point1, const uint colour)
{
    doLine(texture, point0, point1, colour, drawPixel, nullptr);
}

typedef bool (*ComparisonCallback)(TextureData &texture, const QPoint &point, const uint colour0, const uint colour1);
typedef void (*PointCallback)(TextureData &texture, const QPoint &point, const uint colour, const void *const data);
typedef void (*SegmentCallback)(TextureData &texture, const QPoint &point0, const QPoint &point1, const uint colour, void (*pointCallback)(TextureData &texture, const QPoint &point, const uint colour, const void *const data), const void *const data, const bool inclusive);

void Image::point(const QPoint &point, EditingContext *const editingContext)
{
    PointCallback pointCallback = drawPixel;
    pointCallback(*m_imageData, point, editingContext->colourSlot(editingContext->activeColourSlot()), nullptr);
    makeDirty();
}

void Image::stroke(const QPoint &point0, const QPoint &point1, EditingContext *const editingContext)
{
    PointCallback pointCallback = drawPixel;
    SegmentCallback segmentCallback = doLine;
    segmentCallback(*m_imageData, point0, point1, editingContext->colourSlot(editingContext->activeColourSlot()), pointCallback, nullptr, true);
//    undoStack->push(new StrokeCommand(*m_image, a, b));
    makeDirty();
}

void Image::pick(const QPoint &point, EditingContext *const editingContext)
{
    if (m_imageData->rect().contains(point)) {
        if (format() == ImageDataFormat::Indexed) {
            editingContext->setColourSlot(m_imageData->pixel(point), editingContext->activeColourSlot());
        }
        else if (format() == ImageDataFormat::RGBA) {
            editingContext->setColourSlot(m_imageData->pixel(point), editingContext->activeColourSlot());
        }
    }
}
