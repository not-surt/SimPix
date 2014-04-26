#include "colourswatchdelegate.h"
#include "palettemodel.h"
#include "paletteview.h"

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
