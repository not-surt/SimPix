#ifndef PALETTEVIEW_H
#define PALETTEVIEW_H

#include <QListView>
#include <QStyledItemDelegate>
#include "image.h"

class PaletteView : public QListView
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage)
    Q_ENUMS(image)
public:
    explicit PaletteView(QWidget *parent = nullptr);
    Image *image() const;

signals:

public slots:
    void setImage(Image *image);

};

#endif // PALETTEVIEW_H
