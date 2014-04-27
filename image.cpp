#include "image.h"

#include <QDebug>
#include <QColor>
#include <QUndoStack>

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


Image::Image(const Image &image, QObject *parent) :
    QObject(parent), m_fileName(), m_data(image.constData()), m_primaryColour(image.m_primaryColour), m_secondaryColour(image.m_secondaryColour), m_eraserColour(image.m_eraserColour), m_dirty(true)
{

}

Image::Image(const QSize &size, Image::Format format, QObject *parent) :
    QObject(parent), m_fileName(), m_data(size, static_cast<QImage::Format>(format)), m_dirty(true)
{
    if (format == Image::Indexed) {
        m_data.setColor(0, qRgb(0, 0, 0));
        m_data.setColor(1, qRgb(255, 255, 255));
        m_data.fill(0);
        m_primaryColour = 1;
        m_secondaryColour = 0;
    }
    else if (format == Image::RGBA) {
        m_data.fill(qRgba(0, 0, 0, 0));
        m_primaryColour = qRgba(255, 255, 255, 255);
        m_secondaryColour = qRgba(0, 0, 0, 0);
    }
}

Image::Image(const QString &fileName, const char *format, QObject *parent) :
    QObject(parent), m_fileName(fileName), m_data(fileName, format), m_dirty(false)
{
    if (!m_data.isNull()) {
        if (m_data.format() != QImage::Format_Indexed8 && m_data.format() != QImage::Format_ARGB32) {
            if (m_data.colorTable().size() != 0) {
                m_data = m_data.convertToFormat(QImage::Format_Indexed8);
            }
            else {
                m_data = m_data.convertToFormat(QImage::Format_ARGB32);
            }
        }

        if (m_data.format() == QImage::Format_Indexed8) {
            m_primaryColour = m_data.colorTable().size() - 1;
            m_secondaryColour = 0;
        }
        else if (m_data.format() == QImage::Format_ARGB32) {
            m_primaryColour = qRgba(255, 255, 255, 255);
            m_secondaryColour = qRgba(0, 0, 0, 0);
        }
    }
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
        saved = m_data.save(fileName);
        if (saved) {
            m_dirty = false;
            m_fileName = fileName;
        }
    }
    return saved;
}

bool Image::dirty() const
{
    return m_dirty;
}

void drawPixel(QImage &image, const QPoint &point, const uint colour, const void *const data = nullptr)
{
    if (image.rect().contains(point)) {
        image.setPixel(point, colour);
    }
}

void drawRectangularBrush(QImage &image, const QPoint &point, const uint colour, const void *const data)
{
    const uint *const size = (const uint *)data;
}

void drawEllipticalBrush(QImage &image, const QPoint &point, const uint colour, const void *const data)
{
    const uint *const size = (const uint *)data;
}

void drawAngularBrush(QImage &image, const QPoint &point, const uint colour, const void *const data)
{
    const int *const size = (const int *)data;
}

inline int sign(const int val)
{
    return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

void doSpan(QImage &image, const QPoint &point, const uint x1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
{
    const int end = x1 + (inclusive ? 1 : 0);
    for (int x = point.x(); x < end; x++) {
        callback(image, QPoint(x, point.y()), colour, data);
    }
}

void drawSpan(QImage &image, const QPoint &point, const uint x1, const uint colour, const bool inclusive = true)
{
    doSpan(image, point, x1, colour, drawPixel, nullptr, inclusive);
}

void doLine(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
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

void drawLine(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour)
{
    doLine(image, point0, point1, colour, drawPixel, nullptr);
}

void doRectangle(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
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

void drawRectangle(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour) {
    doRectangle(image, point0, point1, colour, drawPixel);
}

void doRectangleFilled(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour, const void *const data), const void *const data = nullptr, const bool inclusive = true)
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

void drawRectangleFilled(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour)
{
    doRectangleFilled(image, point0, point1, colour, drawPixel);
}

void drawEllipse(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour)
{
}

bool indexThresholdCallback(QImage &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data)
{
    const uint threshold = *(uint *)data;
    return (uint)abs(colour0 - colour1) < threshold;
}

bool rgbThresholdCallback(QImage &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data)
{
    const uint *const thresholds = (uint *)data;
    return ((uint)abs(qRed(colour0) - qRed(colour1)) < thresholds[0]) &&
            ((uint)abs(qGreen(colour0) - qGreen(colour1)) < thresholds[1]) &&
            ((uint)abs(qBlue(colour0) - qBlue(colour1)) < thresholds[2]) &&
            ((uint)abs(qAlpha(colour0) - qAlpha(colour1)) < thresholds[3]);
}

bool hslThresholdCallback(QImage &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data)
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

void doFloodfill(QImage &image, const QPoint &point, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour, const void *const data), bool (*comparisonCallback)(QImage &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data) = 0, const void *const data = 0)
{

}

void drawFloodfill(QImage &image, const QPoint &point, const uint colour)
{
    doFloodfill(image, point, colour, drawPixel);
}

typedef bool (*ComparisonCallback)(QImage &image, const QPoint &point, const uint colour0, const uint colour1);
typedef void (*PointCallback)(QImage &image, const QPoint &point, const uint colour, const void *const data);
typedef void (*SegmentCallback)(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*pointCallback)(QImage &image, const QPoint &point, const uint colour, const void *const data), const void *const data, const bool inclusive);

void Image::point(const QPoint &point, const bool secondary)
{
    m_dirty = true;
    PointCallback pointCallback = drawPixel;
    pointCallback(m_data, point, contextColour(Primary), nullptr);
    emit changed(QRegion(QRect(point, QSize(1, 1))));
}

void Image::stroke(const QPoint &point0, const QPoint &point1, const bool secondary)
{
    m_dirty = true;
    PointCallback pointCallback = drawPixel;
    SegmentCallback segmentCallback = doLine;
    segmentCallback(m_data, point0, point1, contextColour(Primary), pointCallback, nullptr, true);
//    undoStack->push(new StrokeCommand(*m_image, a, b));
//    emit changed(QRect(a, b));
    emit changed(QRegion(data().rect()));
}

void Image::pick(const QPoint &point, const bool secondary)
{
    if (m_data.rect().contains(point)) {
        if (format() == Indexed) {
            setContextColour(m_data.pixelIndex(point), Primary);
        }
        else if (format() == RGBA) {
            setContextColour(m_data.pixel(point), Primary);
        }
    }
}

uint Image::contextColour(const int context) const
{
    switch (context) {
    default:
    case Primary:
        return m_primaryColour;
    case Secondary:
        return m_secondaryColour;
    case Eraser:
        return m_eraserColour;
    }
}

void Image::setContextColour(uint colour, const int context)
{
    switch (context) {
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
