#include "util.h"

#include <QFile>
#include <QPainter>
#include <QTextStream>
#include <cstdlib>

QPixmap *swatchBackgroundPixmap = nullptr;

float mod(const float dividend, const float divisor)
{
    return dividend - divisor * floor(dividend / divisor);
}

float randUnit()
{
    return (float)rand() / (float)RAND_MAX;
}

void qTransformFillGlslMat3(const QTransform &transform, GLfloat *const matrix)
{
    matrix[0] = transform.m11();
    matrix[1] = transform.m12();
    matrix[2] = transform.m13();
    matrix[3] = transform.m21();
    matrix[4] = transform.m22();
    matrix[5] = transform.m23();
    matrix[6] = transform.m31();
    matrix[7] = transform.m32();
    matrix[8] = transform.m33();
}

QColor colourAdjustLightness(const QColor &colour, const int offset)
{
    int h, s, l, a;
    colour.getHsl(&h, &s, &l, &a);
    return QColor::fromHsl(h, s, clamp(l + offset, 0, 255), a);
}

QPixmap *generateBackgroundPixmap(const uint size)
{
    QPixmap *pattern = new QPixmap(size, size);
    QPainter painter(pattern);
    const QColor base = QColor(127, 127, 127), light = colourAdjustLightness(base, 16), dark = colourAdjustLightness(base, -16);
    painter.fillRect(QRect(0, 0, pattern->width() / 2, pattern->height() / 2), light);
    painter.fillRect(QRect(pattern->width() / 2, 0, pattern->width() - (pattern->width() / 2), pattern->height() / 2), dark);
    painter.fillRect(QRect(0, pattern->height() / 2, pattern->width() / 2, pattern->height() - (pattern->height() / 2)), dark);
    painter.fillRect(QRect(pattern->width() / 2, pattern->height() / 2, pattern->width() - (pattern->width() / 2), pattern->height() - (pattern->height() / 2)), light);
    return pattern;
}

QString fileToString(QString fileName)
{
    QFile data(fileName);
    if (data.open(QFile::ReadOnly)) {
        return QTextStream(&data).readAll();
    }
    return QString();
}

void drawColourSwatch(QPainter *const painter, const QRect &rect, const QColor &colour)
{
    static const int lightnessOffset = 16;

    painter->save();

    painter->fillRect(rect.adjusted(2, 2, -2, -2), colour.rgb());
    painter->setPen(colourAdjustLightness(colour.rgb(), lightnessOffset));
    painter->drawRect(rect.adjusted(1, 1, -2, -2));
    painter->setPen(colourAdjustLightness(colour.rgb(), -lightnessOffset));
    painter->drawRect(rect.adjusted(0, 0, -1, -1));

    if (colour.alpha() < 255) {
        QRect r = rect.adjusted(0, 0, 1, 1);
        QPolygon alpha;
        alpha.append(r.topLeft());
        alpha.append(r.topRight());
        alpha.append(r.bottomLeft());
        painter->setPen(Qt::NoPen);
        painter->setBrush(*swatchBackgroundPixmap);
        painter->setOpacity((float)(255 - colour.alpha()) / 255.);
        painter->drawConvexPolygon(alpha);
        painter->setBrush(colour);
        painter->drawConvexPolygon(alpha);
    }

    painter->restore();

}
