#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QWidget>
#include <QColor>
#include <ui_palettewidget.h>
#include "scene.h"

namespace Ui {
class PaletteWidget;
}

class PaletteWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Scene *image READ image WRITE setImage)
    Q_ENUMS(image)

public:
    explicit PaletteWidget(QWidget *parent = nullptr);
    ~PaletteWidget();
    Scene *image() const;


signals:
    void colourChanged(const uint colour);

public slots:
    void setImage(Scene *const image);
    void editColour(QModelIndex index);
    void setColour(QModelIndex index);

private:
    Ui::PaletteWidget *ui;
};

#endif // PALETTEWIDGET_H
