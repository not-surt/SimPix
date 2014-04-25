#include "image.h"

#include <QDebug>
#include <QUndoStack>

//StrokeCommand::StrokeCommand(const Image &image, const QPoint &a, const QPoint &b, QUndoCommand *parent)
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

Image::Image() :
    QObject(), m_data(), m_primaryColour(0), m_secondaryColour(0)
{
}

Image::Image(const QSize &size, Image::Format format) :
    QObject(), m_data(size, static_cast<QImage::Format>(format)), m_primaryColour(0), m_secondaryColour(0)
{
}

Image::Image(const QString &fileName, const char *format) :
    QObject(), m_primaryColour(0), m_secondaryColour(0)
{
    QImage image = QImage(fileName, format);
    if (image.format() == QImage::Format_Indexed8 || image.format() == QImage::Format_ARGB32) {
        m_data = image;
    }
    else if (image.colorTable().size() != 0) {
        m_data = image.convertToFormat(QImage::Format_Indexed8);
    }
    else {
        m_data = image.convertToFormat(QImage::Format_ARGB32);
    }
}

Image::Image(Image &image) :
   QObject(),  m_data(image.data()), m_primaryColour(image.m_primaryColour), m_secondaryColour(image.m_secondaryColour)
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

void drawPixel(QImage &image, const QPoint &position, const uint colour)
{
    if (image.rect().contains(position)) {
        image.setPixel(position, colour);
    }
}

inline int sign(const int val)
{
    return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

void doSpan(QImage &image, const QPoint &position, const uint x1, const uint colour, void (*callback)(QImage &image, const QPoint &position, const uint colour), const bool inclusive = true)
{
    const int end = x1 + (inclusive ? 1 : 0);
    for (int x = position.x(); x < end; x++) {
        callback(image, QPoint(x, position.y()), colour);
    }
}

void drawSpan(QImage &image, const QPoint &position, const uint x1, const uint colour, const bool inclusive = true)
{
    doSpan(image, position, x1, colour, drawPixel, inclusive);
}

void doLine(QImage &image, const QPoint &a, const QPoint &b, const uint colour, void (*callback)(QImage &image, const QPoint &position, const uint colour), const bool inclusive = true)
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

void drawLine(QImage &image, const QPoint &a, const QPoint &b, const uint colour)
{
    doLine(image, a, b, colour, drawPixel);
}

void doRectangle(QImage &image, const QPoint &a, const QPoint &b, const uint colour, void (*callback)(QImage &image, const QPoint &position, const uint colour), const bool inclusive = true)
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

void drawRectangle(QImage &image, const QPoint &a, const QPoint &b, const uint colour) {
    doRectangle(image, a, b, colour, drawPixel);
}

void doRectangleFilled(QImage &image, const QPoint &a, const QPoint &b, const uint colour, void (*callback)(QImage &image, const QPoint &position, const uint colour), const bool inclusive = true)
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
        doSpan(image, QPoint(x0, y), x1, colour, callback, true);
    }
}

void drawRectangleFilled(QImage &image, const QPoint &a, const QPoint &b, const uint colour)
{
    doRectangleFilled(image, a, b, colour, drawPixel);
}

void drawEllipse(QImage &image, const QPoint &a, const QPoint &b, const uint colour)
{
}

typedef void (*PointCallback)(QImage &image, const QPoint &position, const uint colour);
typedef void (*SegmentCallback)(QImage &image, const QPoint &a, const QPoint &b, const uint colour, void (*pointCallback)(QImage &image, const QPoint &position, const uint colour), const bool inclusive);

void Image::point(const QPoint &position, const bool secondary)
{
    PointCallback pointCallback = drawPixel;
    uint colour;
    if (format() == Indexed) {
        colour = 1;
    }
    else if (format() == RGBA) {
        colour = qRgb(255, 0, 0);
    }
    pointCallback(m_data, position, currentColour(secondary));
    emit changed(QRegion(QRect(position, QSize(1, 1))));
}

void Image::stroke(const QPoint &a, const QPoint &b, const bool secondary)
{
    PointCallback pointCallback = drawPixel;
    SegmentCallback segmentCallback = doLine;
    segmentCallback(m_data, a, b, currentColour(secondary), pointCallback, true);
//    undoStack->push(new StrokeCommand(*m_image, a, b));
//    emit changed(QRect(a, b));
    emit changed(QRegion(data().rect()));
}

void Image::pick(const QPoint &position, const bool secondary)
{
    if (m_data.rect().contains(position)) {
        if (format() == Indexed) {
            setCurrentColour(m_data.pixelIndex(position), secondary);
        }
        else if (format() == RGBA) {
            setCurrentColour(m_data.pixel(position), secondary);
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
