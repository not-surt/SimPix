#ifndef COLOURSWATCHDELEGATE_H
#define COLOURSWATCHDELEGATE_H

#include <QStyledItemDelegate>

class ColourSwatchDelegate : public QStyledItemDelegate
{
public:
    explicit ColourSwatchDelegate(QWidget *parent = nullptr);
    ~ColourSwatchDelegate();
    // QAbstractItemDelegate interface
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QSize parentSize;
};

#endif // COLOURSWATCHDELEGATE_H
