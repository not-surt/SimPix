#include "colourswatch.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QStyleOptionFocusRect>
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
    drawColourSwatch(&painter, rect(), m_colour);

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().color(QPalette::Background);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}
