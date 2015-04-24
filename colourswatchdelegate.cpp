#include "colourswatchdelegate.h"

#include <QPainter>
#include <QApplication>
#include "util.h"


ColourSwatchDelegate::ColourSwatchDelegate(QWidget *parent) :
    QStyledItemDelegate(parent), parentSize()
{

}

ColourSwatchDelegate::~ColourSwatchDelegate()
{

}

void ColourSwatchDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
        rect.adjust(2, 2, -2, -2);
    }

    QColor color = index.model()->data(index, Qt::DisplayRole).value<QColor>();
    if (option.state & QStyle::State_Selected) {
        QPainterPath path = QPainterPath();
        path.addRect(rect.left(), rect.top(), rect.width(), rect.height());
        rect.adjust(2, 2, -2, -2);
        path.addRect(rect.left(), rect.top(), rect.width(), rect.height());
        painter->fillPath(path, option.palette.highlightedText());
    }
    drawColourSwatch(painter, rect, color);
}

QSize ColourSwatchDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const int size = QApplication::style()->pixelMetric(QStyle::PM_ToolBarIconSize);
//    const int size = (parentSize.width() - 1) / 8; //Bug? Need to subtract one.
    return QSize(size, size);
}
