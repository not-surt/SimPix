#include "image.h"

#include <QDebug>
#include <QUndoStack>

Image::Image() :
    m_data()
{
}

Image::Image(const QSize &size, Image::Format format) :
    m_data(size, static_cast<QImage::Format>(format))
{
    undoStack = new QUndoStack(this);
}

Image::Image(const QString &fileName, const char *format) :
    m_data(fileName, format)
{
}

Image::Image(const QImage &image) :
    m_data(image)
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

void drawPixel(QImage &image, const QPoint &position, const uint index_or_rgb)
{
    if (image.rect().contains(position)) {
        image.setPixel(position, index_or_rgb);
    }
}

inline int sign(const int val)
{
    return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

void doSpan(QImage &image, const QPoint &position, const uint x1, const uint index_or_rgb, void (*callback)(QImage &image, const QPoint &position, const uint index_or_rgb), const bool inclusive = true)
{
    const int end = x1 + (inclusive ? 1 : 0);
    for (int x = position.x(); x < end; x++) {
        callback(image, QPoint(x, position.y()), index_or_rgb);
    }
}

void drawSpan(QImage &image, const QPoint &position, const uint x1, const uint index_or_rgb, const bool inclusive = true)
{
    doSpan(image, position, x1, index_or_rgb, drawPixel, inclusive);
}

void doLine(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb, void (*callback)(QImage &image, const QPoint &position, const uint index_or_rgb), const bool inclusive = true)
{
    QPoint delta = b - a;
    const int stepX = sign(delta.x()), stepY = sign(delta.y());
    int sumStepX = abs(delta.y()), sumStepY = abs(delta.x());
    int x = a.x(), y = a.y();
    if (sumStepX > 0 || sumStepY > 0) {
        if (sumStepX == 0) {
            sumStepX = sumStepY;
        }
        if (sumStepY == 0) {
            sumStepY = sumStepX;
        }
        const int limit = sumStepX * sumStepY;
        int sumX = sumStepX, sumY = sumStepY;
//        qDebug() << sumStepX << "," << sumStepY << " | " << limit;
        do {
//            qDebug() << sumX << "," << sumY;
            callback(image, QPoint(x, y), index_or_rgb);
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
        callback(image, QPoint(x, y), index_or_rgb);
    }
}

void drawLine(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb)
{
    doLine(image, a, b, index_or_rgb, drawPixel);
}

void doRectangle(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb, void (*callback)(QImage &image, const QPoint &position, const uint index_or_rgb), const bool inclusive = true)
{
    int x0, x1;
    if (a.x() < b.x()) {
        x0 = a.x();
        x1 = b.x();
    }
    else {
        x0 = b.x();
        x1 = a.x();
    }
    int y0, y1;
    if (a.y() < b.y()) {
        y0 = a.y();
        y1 = b.y();
    }
    else {
        y0 = b.y();
        y1 = a.y();
    }
    if (!inclusive) {
        x1--;
        y1--;
    }
    int deltaX = x1 - x0;
    int deltaY = y1 - y0;
    doSpan(image, QPoint(x0, y0), x1, index_or_rgb, callback, true);
    if (deltaY > 0) {
        if (deltaY > 1) {
            for (int y = y0; y <= y1; y++) {
                callback(image, QPoint(x0, y), index_or_rgb);
                if (deltaX > 0) {
                    callback(image, QPoint(x1, y), index_or_rgb);
                }
            }
        }
        doSpan(image, QPoint(x0, y1), x1, index_or_rgb, callback, true);
    }
}

void drawRectangle(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb) {
    doRectangle(image, a, b, index_or_rgb, drawPixel);
}

void doRectangleFilled(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb, void (*callback)(QImage &image, const QPoint &position, const uint index_or_rgb), const bool inclusive = true)
{
    int x0, x1;
    if (a.x() < b.x()) {
        x0 = a.x();
        x1 = b.x();
    }
    else {
        x0 = b.x();
        x1 = a.x();
    }
    int y0, y1;
    if (a.y() < b.y()) {
        y0 = a.y();
        y1 = b.y();
    }
    else {
        y0 = b.y();
        y1 = a.y();
    }
    if (!inclusive) {
        x1--;
        y1--;
    }
    for (int y = y0; y <= y1; y++) {
        doSpan(image, QPoint(x0, y), x1, index_or_rgb, callback, true);
    }
}

void drawRectangleFilled(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb) {
    doRectangleFilled(image, a, b, index_or_rgb, drawPixel);
}

void drawEllipse(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb)
{
}

typedef void (*PointCallback)(QImage &image, const QPoint &position, const uint index_or_rgb);
typedef void (*SegmentCallback)(QImage &image, const QPoint &a, const QPoint &b, const uint index_or_rgb, void (*pointCallback)(QImage &image, const QPoint &position, const uint index_or_rgb), const bool inclusive);

void Image::stroke(const QPoint &a, const QPoint &b)
{
    PointCallback pointCallback = drawPixel;
    SegmentCallback segmentCallback = doLine;
    uint colour;
    if (format() == Indexed) {
        colour = 1;
    }
    else if (format() == RGBA) {
        colour = qRgb(255, 0, 0);
    }
    segmentCallback(m_data, a, b, colour, pointCallback, true);
//    undoStack->push(new StrokeCommand(*m_image, a, b));
    emit changed();
}
