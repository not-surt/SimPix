#include "colourdelegate.h"

#include <QPainter>


ColourDelegate::ColourDelegate(QWidget *parent) :
    QStyledItemDelegate(parent), parentSize()
{

}

ColourDelegate::~ColourDelegate()
{

}

void ColourDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
//    QRect rect = option.rect.adjusted(1, 1, -1, -1);
    QRect rect = option.rect;
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
        rect.adjust(2, 2, -2, -2);
    }

    QColor color = index.model()->data(index, Qt::DisplayRole).value<QColor>();
    painter->fillRect(rect.adjusted(2, 2, -2, -2), color);
    if (option.state & QStyle::State_Selected) {
        QPainterPath path = QPainterPath();
        path.addRect(rect.left(), rect.top(), rect.width(), rect.height());
        rect.adjust(2, 2, -2, -2);
        path.addRect(rect.left(), rect.top(), rect.width(), rect.height());
        painter->fillPath(path, option.palette.highlightedText());
    }
    else {
        painter->setPen(color.lighter());
        painter->drawRect(rect.adjusted(1, 1, -2, -2));
        painter->setPen(color.darker());
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
    }
    painter->restore();
}

QSize ColourDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(24, 24);
//    int size = (parentSize.width() - 1) / 8; //Bug? Need to subtract one.
    //    return QSize(size, size);
}
