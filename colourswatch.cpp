#include "colourswatch.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include "util.h"

ColourSwatch::ColourSwatch(QWidget *parent) :
    QWidget(parent)
{
}

void ColourSwatch::setColour(const QColor &arg)
{
    if (m_colour != arg) {
        m_colour = arg;
        emit colourChanged(arg);
    }
}


QSize ColourSwatch::sizeHint() const
{
    const int size = QApplication::style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    return QSize(size, size);
}

const QColor &ColourSwatch::colour() const
{
    return m_colour;
}

void ColourSwatch::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
//    painter.fillRect(rect().adjusted(2, 2, -2, -2), m_colour);
//    painter.setPen(m_colour.lighter(125));
//    painter.drawRect(rect().adjusted(1, 1, -2, -2));
//    painter.setPen(m_colour.darker(125));
//    painter.drawRect(rect().adjusted(0, 0, -1, -1));
    m_colour = QColor(255, 127, 0, 127);
    painter.setPen(Qt::NoPen);
    QRect r = rect().adjusted(0, 0, 1, 1);
    QPolygon alpha;
    alpha.append(r.topLeft());
    alpha.append(r.topRight());
    alpha.append(r.bottomLeft());
    painter.setBrush(*swatchBackgroundPixmap);
    painter.drawConvexPolygon(alpha);
    painter.setBrush(m_colour);
    painter.drawConvexPolygon(alpha);
    QPolygon solid;
    solid.append(r.topRight());
    solid.append(r.bottomRight());
    solid.append(r.bottomLeft());
    painter.setBrush(QColor(m_colour.rgb()));
    painter.drawConvexPolygon(solid);
}
