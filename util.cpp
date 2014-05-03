#include "util.h"

#include <QFile>
#include <QPainter>
#include <QTextStream>

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

QPixmap *generateBackgroundPixmap(const uint size)
{
    QPixmap *pattern = new QPixmap(size, size);
    QPainter painter(pattern);
    const QColor base = QColor(127, 127, 127), light = base.lighter(125), dark = base.darker(125);
    painter.fillRect(QRect(0, 0, pattern->width() / 2, pattern->height() / 2), light);
    painter.fillRect(QRect(pattern->width() / 2, 0, pattern->width() - (pattern->width() / 2), pattern->height() / 2), dark);
    painter.fillRect(QRect(0, pattern->height() / 2, pattern->width() / 2, pattern->height() - (pattern->height() / 2)), dark);
    painter.fillRect(QRect(pattern->width() / 2, pattern->height() / 2, pattern->width() - (pattern->width() / 2), pattern->height() - (pattern->height() / 2)), light);
    return pattern;
}

QPixmap *canvasBackgroundPixmap;
QPixmap *swatchBackgroundPixmap;

QString fileToString(QString fileName)
{
    QFile data(fileName);
    if (data.open(QFile::ReadOnly)) {
        return QTextStream(&data).readAll();
    }
    return QString();
}
