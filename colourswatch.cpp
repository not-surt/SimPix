#include "colourswatch.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QStyleOptionFocusRect>
#include "util.h"

ColourSwatch::ColourSwatch(QWidget *parent) :
    QAbstractButton(parent), m_colour()
{
    setFocusPolicy(Qt::StrongFocus);
//    focusFrame = new QFocusFrame(this);
//    focusFrame->setStyle(this->style());
//    focusFrame->setWidget(this);
}

void ColourSwatch::setColour(const Colour &colour)
{
    if (m_colour != colour) {
        m_colour = colour;
        update();
        emit colourChanged(colour);
    }
}


QSize ColourSwatch::sizeHint() const
{
    const int size = QApplication::style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    return QSize(size, size);
}

const Colour &ColourSwatch::colour() const
{
    return m_colour;
}

void ColourSwatch::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawColourSwatch(&painter, rect(), QColor(m_colour.r, m_colour.g, m_colour.b, m_colour.a));

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().color(QPalette::Background);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}
