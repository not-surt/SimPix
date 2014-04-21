#ifndef COLOURDELEGATE_H
#define COLOURDELEGATE_H
#include <QStyledItemDelegate>

class ColourDelegate : public QStyledItemDelegate
{
public:
    explicit ColourDelegate(QWidget *parent = 0);
    ~ColourDelegate();
    // QAbstractItemDelegate interface
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QSize parentSize;
};

#endif // COLOURDELEGATE_H
