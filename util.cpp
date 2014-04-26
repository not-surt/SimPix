#include "util.h"

#include <QPainter>

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

QPixmap *backgroundPixmap;
