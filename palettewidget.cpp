#include "palettewidget.h"
#include "ui_palettewidget.h"

#include <QItemDelegate>
#include <QPainter>
#include <QStringListModel>

PaletteWidget::PaletteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaletteWidget)
{
    ui->setupUi(this);
    PaletteModel *model = new PaletteModel(0);
    ui->listView->setModel(model);
    QAbstractItemDelegate *old = ui->listView->itemDelegate();
    ui->listView->setItemDelegate(new ColourDelegate());
    delete old;
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setDragEnabled(true);
    ui->listView->setDropIndicatorShown(true);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setFlow(QListView::LeftToRight);
    ui->listView->setWrapping(true);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setSelectionRectVisible(false);
    ui->listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->listView->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
}

PaletteWidget::~PaletteWidget()
{
    delete ui;
}

QImage *PaletteWidget::image() const
{
    return ((PaletteModel *)(ui->listView->model()))->image();
}

void PaletteWidget::setImage(QImage *arg)
{
    ((PaletteModel *)(ui->listView->model()))->setImage(arg);
}


PaletteWidget::ColourDelegate::ColourDelegate(QWidget *parent) :
    QStyledItemDelegate(parent), parentSize()
{

}

PaletteWidget::ColourDelegate::~ColourDelegate()
{

}

void PaletteWidget::ColourDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

QSize PaletteWidget::ColourDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(24, 24);
//    int size = (parentSize.width() - 1) / 8; //Bug? Need to subtract one.
//    return QSize(size, size);
}
