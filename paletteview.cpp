#include "colourswatchdelegate.h"
#include "palettemodel.h"
#include "paletteview.h"

#include <QMouseEvent>
#include <QPainter>

PaletteView::PaletteView(QWidget *parent) :
    QListView(parent)
{
    PaletteModel *model = new PaletteModel(0);
    setModel(model);
    QAbstractItemDelegate *old = itemDelegate();
    setItemDelegate(new ColourSwatchDelegate());
    delete old;
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(true);
    setResizeMode(QListView::Adjust);
    setSelectionRectVisible(false);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
}

Image *PaletteView::image() const
{
    return ((PaletteModel *)model())->image();
}

void PaletteView::setImage(Image *image)
{
    ((PaletteModel *)model())->setImage(image);
}


void PaletteView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
//        emit customContextMenuRequested(event->pos());
    }
    else
        QListView::mousePressEvent(event);
}

void PaletteView::mouseReleaseEvent(QMouseEvent *event)
{
    QListView::mouseReleaseEvent(event);
}

void PaletteView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QListView::mouseDoubleClickEvent(event);
}

void PaletteView::mouseMoveEvent(QMouseEvent *event)
{
    QListView::mouseMoveEvent(event);
}
