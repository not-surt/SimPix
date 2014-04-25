#include "image.h"

#include <QDebug>
#include <QColor>
#include <QUndoStack>

//StrokeCommand::StrokeCommand(const Image &image, const QPoint &point0, const QPoint &point1, QUndoCommand *parent)
//    : image(image), a(a), b(b)
//{
//    QRect rect = QRect(a, QSize(1, 1)).united(QRect(b, QSize(1, 1)));
//    dirty = Image(image.copy(rect));
//}

//StrokeCommand::~StrokeCommand()
//{
//}

//void StrokeCommand::undo()
//{

//}

//void StrokeCommand::redo()
//{

//}

Image::Image(const QSize &size, Image::Format format, QObject *parent) :
    QObject(parent), m_data(size, static_cast<QImage::Format>(format))
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
    QObject(parent), m_data(fileName, format)
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

Image::Image(Image &image, QObject *parent) :
   QObject(parent),  m_data(image.data()), m_primaryColour(image.m_primaryColour), m_secondaryColour(image.m_secondaryColour)
{

}

Image::~Image()
{
    delete undoStack;
}

QImage &Image::data()
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

uint Image::currentColour(const bool secondary) const
{
    return m_primaryColour;
    if (!secondary) {
        return m_primaryColour;
    }
    else {
        return m_secondaryColour;
    }
}

void drawPixel(QImage &image, const QPoint &point, const uint colour)
{
    if (image.rect().contains(point)) {
        image.setPixel(point, colour);
    }
}

inline int sign(const int val)
{
    return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

void doSpan(QImage &image, const QPoint &point, const uint x1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour), const bool inclusive = true)
{
    const int end = x1 + (inclusive ? 1 : 0);
    for (int x = point.x(); x < end; x++) {
        callback(image, QPoint(x, point.y()), colour);
    }
}

void drawSpan(QImage &image, const QPoint &point, const uint x1, const uint colour, const bool inclusive = true)
{
    doSpan(image, point, x1, colour, drawPixel, inclusive);
}

void doLine(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour), const bool inclusive = true)
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
            callback(image, QPoint(x, y), colour);
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
        callback(image, QPoint(x, y), colour);
    }
}

void drawLine(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour)
{
    doLine(image, point0, point1, colour, drawPixel);
}

void doRectangle(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour), const bool inclusive = true)
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
    doSpan(image, QPoint(x0, y0), x1, colour, callback, true);
    if (deltaY > 0) {
        if (deltaY > 1) {
            for (int y = y0; y <= y1; y++) {
                callback(image, QPoint(x0, y), colour);
                if (deltaX > 0) {
                    callback(image, QPoint(x1, y), colour);
                }
            }
        }
        doSpan(image, QPoint(x0, y1), x1, colour, callback, true);
    }
}

void drawRectangle(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour) {
    doRectangle(image, point0, point1, colour, drawPixel);
}

void doRectangleFilled(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour), const bool inclusive = true)
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
        doSpan(image, QPoint(x0, y), x1, colour, callback, true);
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

void doFloodfill(QImage &image, const QPoint &point, const uint colour, void (*callback)(QImage &image, const QPoint &point, const uint colour), bool (*comparisonCallback)(QImage &image, const QPoint &point, const uint colour0, const uint colour1, const void *const data) = 0, const void *const data = 0)
{

}

void drawFloodfill(QImage &image, const QPoint &point, const uint colour)
{
    doFloodfill(image, point, colour, drawPixel);
}

typedef bool (*ComparisonCallback)(QImage &image, const QPoint &point, const uint colour0, const uint colour1);
typedef void (*PointCallback)(QImage &image, const QPoint &point, const uint colour);
typedef void (*SegmentCallback)(QImage &image, const QPoint &point0, const QPoint &point1, const uint colour, void (*pointCallback)(QImage &image, const QPoint &point, const uint colour), const bool inclusive);

void Image::point(const QPoint &point, const bool secondary)
{
    PointCallback pointCallback = drawPixel;
    pointCallback(m_data, point, currentColour(secondary));
    emit changed(QRegion(QRect(point, QSize(1, 1))));
}

void Image::stroke(const QPoint &point0, const QPoint &point1, const bool secondary)
{
    PointCallback pointCallback = drawPixel;
    SegmentCallback segmentCallback = doLine;
    segmentCallback(m_data, point0, point1, currentColour(secondary), pointCallback, true);
//    undoStack->push(new StrokeCommand(*m_image, a, b));
//    emit changed(QRect(a, b));
    emit changed(QRegion(data().rect()));
}

void Image::pick(const QPoint &point, const bool secondary)
{
    if (m_data.rect().contains(point)) {
        if (format() == Indexed) {
            setCurrentColour(m_data.pixelIndex(point), secondary);
        }
        else if (format() == RGBA) {
            setCurrentColour(m_data.pixel(point), secondary);
        }
    }
}

void Image::setCurrentColour(uint arg, const bool secondary)
{
    if (!secondary) {
        if (m_primaryColour != arg) {
            m_primaryColour = arg;
            emit currentColourChanged(m_primaryColour, false);
        }
    }
    else {
        if (m_secondaryColour != arg) {
            m_secondaryColour = arg;
            emit currentColourChanged(m_secondaryColour, false);
        }
    }
}
