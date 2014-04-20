#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include "palettemodel.h"

#include <QWidget>
#include <QDebug>
#include <ui_palettewidget.h>
#include <QStyledItemDelegate>

namespace Ui {
class PaletteWidget;
}

class PaletteWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QImage *image READ image WRITE setImage)
    Q_ENUMS(image)

public:
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
    explicit PaletteWidget(QWidget *parent = 0);
    ~PaletteWidget();
    QImage *image() const;


signals:

public slots:
    void setImage(QImage * arg);

private:
    Ui::PaletteWidget *ui;
};

#endif // PALETTEWIDGET_H
