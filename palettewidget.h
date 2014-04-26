#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QWidget>
#include <ui_palettewidget.h>
#include "image.h"

namespace Ui {
class PaletteWidget;
}

class PaletteWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage)
    Q_ENUMS(image)

public:
    explicit PaletteWidget(QWidget *parent = nullptr);
    ~PaletteWidget();
    Image *image() const;


signals:

public slots:
    void setImage(Image *const image);
    void editColour(QModelIndex index);
    void setPrimaryColour(QModelIndex index);
    void setSecondaryColour(QModelIndex index);

private:
    Ui::PaletteWidget *ui;
};

#endif // PALETTEWIDGET_H
