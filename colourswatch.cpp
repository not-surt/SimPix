#include "colourswatch.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include "util.h"

ColourSwatch::ColourSwatch(QWidget *parent) :
    QAbstractButton(parent), m_colour(QColor(0, 0, 0, 0))
{
}

void ColourSwatch::setColour(const QColor &arg)
{
    if (m_colour != arg) {
        m_colour = arg;
        update();
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
