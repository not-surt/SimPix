#include "image.h"

#include <QDebug>
#include <QColor>
#include <QUndoStack>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFramebufferObject>
#include "application.h"

StrokeCommand::StrokeCommand(const Image &image, const QPoint &point0, const QPoint &point1, QUndoCommand *parent)
    : QUndoCommand(parent), image(image), point0(point0), point1(point1)
{
//    QRect rect = QRect(a, QSize(1, 1)).united(QRect(b, QSize(1, 1)));
//    dirty = Image(image.copy(rect));
}

StrokeCommand::~StrokeCommand()
{
}

void StrokeCommand::undo()
{

}

void StrokeCommand::redo()
{

}


const ImageDataFormatDefinition IMAGE_DATA_FORMATS[] = {
    {ImageDataFormat::Indexed, "Indexed", GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte)},
    {ImageDataFormat::RGBA, "RGBA", GL_RGBA8UI, GL_BGRA_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte) * 4},
    {ImageDataFormat::Invalid, "", 0, 0, 0, 0}
};

TextureData::TextureData(const QSize &size, const ImageDataFormat _format, const GLubyte *const data) :
    m_size(size), m_format(_format)
{
    initializeOpenGLFunctions();

    const ImageDataFormatDefinition *const format = &IMAGE_DATA_FORMATS[(int)_format];

    glGenTextures((GLsizei)1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, format->internalFormat, size.width(), size.height(), 0, format->format, format->glEnum, data);

    glGenFramebuffers((GLsizei)1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
    const GLenum BUFFERS[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, BUFFERS);
    glViewport(0, 0, size.width(), size.height());
}

TextureData::~TextureData()
{
    initializeOpenGLFunctions();

    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_framebuffer);
}

uint TextureData::pixel(const QPoint &position)
{
    initializeOpenGLFunctions();

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    uint colour = 0;
    glReadPixels(position.x(), position.y(), 1, 1, IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, &colour);
    return colour;
}

void TextureData::setPixel(const QPoint &position, const uint colour)
{
    initializeOpenGLFunctions();

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, position.x(), position.y(), 1, 1, IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, &colour);
}

GLuint TextureData::texture() const
{
    return m_texture;
}

QSize TextureData::size() const
{
    return m_size;
}

ImageDataFormat TextureData::format() const
{
    return m_format;
}

GLuint TextureData::framebuffer() const
{
    return m_framebuffer;
}

GLubyte *TextureData::readData(GLubyte *const _data)
{
    initializeOpenGLFunctions();

    uchar *data = (_data != nullptr) ? _data : new uchar[m_size.width() * m_size.height() * IMAGE_DATA_FORMATS[(int)m_format].size];
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glReadPixels(0, 0, m_size.width(), m_size.height(), IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, data);
    return data;
}

void TextureData::writeData(const GLubyte *const data)
{
    initializeOpenGLFunctions();

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_size.width(), m_size.height(), IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, data);
}

PaletteData::PaletteData(const GLuint length, const GLubyte *const data) :
    TextureData(QSize(length, 1), ImageDataFormat::RGBA, data)
{

}

uint PaletteData::colour(const uint index)
{
    return pixel(QPoint(index, 0));
}

void PaletteData::setColour(const uint index, uint colour)
{
    setPixel(QPoint(index, 0), colour);
}

uint PaletteData::length() const
{
    return m_size.width();
}

ImageData::ImageData(const QSize &size, const ImageDataFormat format, const GLubyte *const data) :
    TextureData(size, format, data)
{
    initializeOpenGLFunctions();

    glGenBuffers((GLsizei)1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    const GLfloat vertices[][3] = {
        {0.f, 0.f, 0.f},
        {(GLfloat)size.width(), 0.f, 0.f},
        {(GLfloat)size.width(), (GLfloat)size.height(), 0.f},
        {0.f, (GLfloat)size.height(), 0.f},
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
}

ImageData::~ImageData()
{
    initializeOpenGLFunctions();

    glDeleteBuffers(1, &m_vertexBuffer);
}

GLuint ImageData::vertexBuffer() const
{
    return m_vertexBuffer;
}

//Image::Image(const Image &image, QObject *parent) :
//    QObject(parent), m_fileName(), m_data(image.constData()), m_primaryColour(image.m_primaryColour), m_secondaryColour(image.m_secondaryColour), m_eraserColour(image.m_eraserColour), m_dirty(true), m_activeContextColour(Image::Primary)
//{
//    // m_imageData
//}

//Image::Image(const QSize &size, Image::Format format, QObject *parent) :
//    QObject(parent), m_fileName(), m_data(size, static_cast<QImage::Format>(format)), m_dirty(true)
//{
//    // m_imageData
//    if (format == Image::Indexed) {
//        m_data.setColor(0, qRgb(0, 0, 0));
//        m_data.setColor(1, qRgb(255, 255, 255));
//        m_data.fill(0);
//        m_primaryColour = 1;
//        m_secondaryColour = m_eraserColour = 0;
//    }
//    else if (format == Image::RGBA) {
//        m_data.fill(qRgba(0, 0, 0, 0));
//        m_primaryColour = qRgba(255, 255, 255, 255);
//        m_secondaryColour = m_eraserColour = qRgba(0, 0, 0, 0);
//    }
//}

Image::Image(const QString &fileName, const char *fileFormat, QObject *parent) :
//    QObject(parent), m_fileName(fileName), m_data(fileName, format), m_dirty(false)
    QObject(parent), m_fileName(fileName), m_dirty(false)
{
//    // m_imageData
//    if (!m_data.isNull()) {
//        if (m_data.format() != QImage::Format_Indexed8 && m_data.format() != QImage::Format_ARGB32) {
//            if (m_data.colorTable().size() != 0) {
//                m_data = m_data.convertToFormat(QImage::Format_Indexed8);
//            }
//            else {
//                m_data = m_data.convertToFormat(QImage::Format_ARGB32);
//            }
//        }

//        if (m_data.format() == QImage::Format_Indexed8) {
//            m_primaryColour = m_data.colorTable().size() - 1;
//            m_secondaryColour = 0;
//        }
//        else if (m_data.format() == QImage::Format_ARGB32) {
//            m_primaryColour = qRgba(255, 255, 255, 255);
//            m_secondaryColour = qRgba(0, 0, 0, 0);
//        }
//    }

    ((Application *)qApp)->contextMakeCurrent();
    QImage image(fileName, fileFormat);

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

    m_data = image; //////////////////////

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
        m_paletteData = new PaletteData(image.colorTable().size(), (GLubyte *)image.colorTable().constData());
    }

    switch (format) {
    case ImageDataFormat::Indexed:
        if (m_paletteData && m_paletteData->length() > 0) {
            m_primaryColour = m_paletteData->colour(m_paletteData->length() - 1);
        }
        else {
            m_primaryColour = 255;
        }
        m_secondaryColour = 0;
        break;
    case ImageDataFormat::RGBA:
        m_primaryColour = qRgba(255, 255, 255, 255);
        m_secondaryColour = qRgba(0, 0, 0, 0);
        break;
    }

    m_imageData = new ImageData(image.size(), format, image.constBits());

}

Image::~Image()
{
    delete undoStack;
}

QImage &Image::data()
{
    return m_data;
}

const QImage &Image::constData() const
{
    return m_data;
}

Image::Format Image::format() const
{
    switch (m_data.format()) {
    case QImage::Format_Indexed8:
        return Indexed;
    case QImage::Format_ARGB32:
        return RGBA;
    default:
        return Invalid;
    }
}

const QString &Image::fileName() const
{
    return m_fileName;
}

bool Image::save(QString fileName)
{

    bool saved = false;
    if (fileName.isNull()) {
        fileName = m_fileName;
    }
    if (!fileName.isNull()) {
        ((Application *)qApp)->contextMakeCurrent();
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
            std::vector<QRgb> vector((QRgb *)palette, (QRgb *)(palette + m_paletteData->length()));
            qImage.setColorTable(QVector<QRgb>::fromStdVector(vector));
            delete palette;
        }
        saved = qImage.save(fileName);
        if (saved) {
            m_dirty = false;
            m_fileName = fileName;
        }
        delete data;
    }
    return saved;
}

bool Image::dirty() const
{
    return m_dirty;
}

ImageData *Image::imageData()
{
    return m_imageData;
}

PaletteData *Image::paletteData()
{
    return m_paletteData;
}

bool Image::isIndexed()
{
    return m_data.format() == QImage::Format_Indexed8;
}

Image::ContextColour Image::activeContextColour() const
{
    return m_activeContextColour;
}

void drawPixel(ImageData &image, const QPoint &point, const uint colour, const void *const data = nullptr)
{

    if (QRect(QPoint(0, 0), image.size()).contains(point)) {
        image.setPixel(point, colour);
    }
}

void drawRectangularBrush(QImage &image, const QPoint &point, const uint colour, const void *const data)
{
    const uint *const size = (const uint *)data;
}

void drawEllipticalBrush(ImageData &image, const QPoint &point, const uint colour, const void *const data)
{
    const uint *const size = (const uint *)data;
}

void drawAngularBrush(ImageData &image, const QPoint &point, const uint colour, const void *const data)
{
    const int *const size = (const int *)data;
}

inline int sign(const int val)
{
    return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

void doSpan(ImageData &image, const QPoint &point, const uint x1, const uint colour, void (*callback)(ImageData &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
{
    const int end = x1 + (inclusive ? 1 : 0);
    for (int x = point.x(); x < end; x++) {
        callback(image, QPoint(x, point.y()), colour, data);
    }
}

void drawSpan(ImageData &image, const QPoint &point, const uint x1, const uint colour, const bool inclusive = true)
{
    doSpan(image, point, x1, colour, drawPixel, nullptr, inclusive);
}

void doLine(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(ImageData &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
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
            callback(image, QPoint(x, y), colour, data);
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
        callback(image, QPoint(x, y), colour, data);
    }
}

void drawLine(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour)
{
    doLine(image, point0, point1, colour, drawPixel, nullptr);
}

void doRectangle(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(ImageData &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
{
    int x0, x1;
    if (point0.x() < point1.x()) {
        x0 = point0.x();
        x1 = point1.x();
    }
    else {
        x0 = point1.x();
        x1 = point0.x();
    }
    int y0, y1;
    if (point0.y() < point1.y()) {
        y0 = point0.y();
        y1 = point1.y();
    }
    else {
        y0 = point1.y();
        y1 = point0.y();
    }
    if (!inclusive) {
        x1--;
        y1--;
    }
    int deltaX = x1 - x0;
    int deltaY = y1 - y0;
    doSpan(image, QPoint(x0, y0), x1, colour, callback, data, true);
    if (deltaY > 0) {
        if (deltaY > 1) {
            for (int y = y0; y <= y1; y++) {
                callback(image, QPoint(x0, y), colour, data);
                if (deltaX > 0) {
                    callback(image, QPoint(x1, y), colour, data);
                }
            }
        }
        doSpan(image, QPoint(x0, y1), x1, colour, callback, data, true);
    }
}

void drawRectangle(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour) {
    doRectangle(image, point0, point1, colour, drawPixel);
}

void doRectangleFilled(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(ImageData &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
{
    int x0, x1;
    if (point0.x() < point1.x()) {
        x0 = point0.x();
        x1 = point1.x();
    }
    else {
        x0 = point1.x();
        x1 = point0.x();
    }
    int y0, y1;
    if (point0.y() < point1.y()) {
        y0 = point0.y();
        y1 = point1.y();
    }
    else {
        y0 = point1.y();
        y1 = point0.y();
    }
    if (!inclusive) {
        x1--;
        y1--;
    }
    for (int y = y0; y <= y1; y++) {
        doSpan(image, QPoint(x0, y), x1, colour, callback, data, true);
    }
}

void drawRectangleFilled(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour)
{
    doRectangleFilled(image, point0, point1, colour, drawPixel);
}

void drawEllipse(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour)
{
}

bool indexThresholdCallback(ImageData &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data)
{
    const uint threshold = *(uint *)data;
    return (uint)abs(colour0 - colour1) < threshold;
}

bool rgbThresholdCallback(ImageData &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data)
{
    const uint *const thresholds = (uint *)data;
    return ((uint)abs(qRed(colour0) - qRed(colour1)) < thresholds[0]) &&
            ((uint)abs(qGreen(colour0) - qGreen(colour1)) < thresholds[1]) &&
            ((uint)abs(qBlue(colour0) - qBlue(colour1)) < thresholds[2]) &&
            ((uint)abs(qAlpha(colour0) - qAlpha(colour1)) < thresholds[3]);
}

bool hslThresholdCallback(ImageData &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data)
{
    const uint *const thresholds = (uint *)data;
    int h0, s0, l0, a0, h1, s1, l1, a1;
    QColor::fromRgba(colour0).getHsl(&h0, &s0, &l0, &a0);
    QColor::fromRgba(colour1).getHsl(&h1, &s1, &l1, &a1);
    return ((uint)abs(qRed(h0) - qRed(h1)) < thresholds[0]) &&
            ((uint)abs(qGreen(s0) - qGreen(s1)) < thresholds[1]) &&
            ((uint)abs(qBlue(l0) - qBlue(l1)) < thresholds[2]) &&
            ((uint)abs(qAlpha(a0) - qAlpha(a1)) < thresholds[3]);
}

void doFloodfill(ImageData &image, const QPoint &point, const uint colour, void (*callback)(ImageData &image, const QPoint &point, const uint colour, const void *const data), bool (*comparisonCallback)(ImageData &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data) = 0, const void *const data = 0)
{

}

void drawFloodfill(ImageData &image, const QPoint &point, const uint colour)
{
    doFloodfill(image, point, colour, drawPixel);
}

typedef bool (*ComparisonCallback)(ImageData &image, const QPoint &point, const uint colour0, const uint colour1);
typedef void (*PointCallback)(ImageData &image, const QPoint &point, const uint colour, const void *const data);
typedef void (*SegmentCallback)(ImageData &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*pointCallback)(ImageData &image, const QPoint &point, const uint colour, const void *const data), const void *const data, const bool inclusive);

void Image::point(const QPoint &point, const ContextColour contextColour)
{
    m_dirty = true;
    PointCallback pointCallback = drawPixel;
    pointCallback(*m_imageData, point, this->contextColour(contextColour), nullptr);
    emit changed(QRegion(QRect(point, QSize(1, 1))));
}

void Image::stroke(const QPoint &point0, const QPoint &point1, const ContextColour contextColour)
{
    m_dirty = true;
    PointCallback pointCallback = drawPixel;
    SegmentCallback segmentCallback = doLine;
    segmentCallback(*m_imageData, point0, point1, this->contextColour(contextColour), pointCallback, nullptr, true);
//    undoStack->push(new StrokeCommand(*m_image, a, b));
//    emit changed(QRect(a, b));
    emit changed(QRegion(data().rect()));
}

void Image::pick(const QPoint &point, const ContextColour contextColour)
{
    if (m_data.rect().contains(point)) {
        if (format() == Indexed) {
            setContextColour(m_imageData->pixel(point), contextColour);
        }
        else if (format() == RGBA) {
            setContextColour(m_imageData->pixel(point), contextColour);
        }
    }
}

uint Image::contextColour(const ContextColour contextColour) const
{
    switch (contextColour) {
    default:
    case Primary:
        return m_primaryColour;
    case Secondary:
        return m_secondaryColour;
    case Eraser:
        return m_eraserColour;
    }
}

void Image::setContextColour(uint colour, const ContextColour contextColour)
{
    switch (contextColour) {
    default:
    case Primary:
        if (m_primaryColour != colour) {
            m_primaryColour = colour;
            emit contextColourChanged(m_primaryColour, Primary);
        }
        break;
    case Secondary:
        if (m_secondaryColour != colour) {
            m_secondaryColour = colour;
            emit contextColourChanged(m_secondaryColour, Secondary);
        }
        break;
    case Eraser:
        if (m_eraserColour != colour) {
            m_eraserColour = colour;
            emit contextColourChanged(m_eraserColour, Eraser);
        }
        break;
    }
}

void Image::setFileName(const QString &fileName)
{
    if (m_fileName != fileName) {
        m_fileName = fileName;
        emit fileNameChanged(fileName);
    }
}

void Image::setActiveContextColour(const ContextColour contextColour)
{
    if (m_activeContextColour != contextColour) {
        m_activeContextColour = contextColour;
        emit activeContextColourChanged(contextColour);
    }
}
