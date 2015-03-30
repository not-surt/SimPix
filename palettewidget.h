#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QWidget>
#include <QColor>
#include <ui_palettewidget.h>
#include "editingcontext.h"

namespace Ui {
class PaletteWidget;
}

class PaletteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaletteWidget(QWidget *parent = nullptr);
    ~PaletteWidget();
    EditingContext *editingContext() const;


signals:
    void colourChanged(const uint colour);

public slots:
    void setEditingContext(EditingContext *const editingContext);
    void editColour(QModelIndex index);
    void setColour(QModelIndex index);

private:
    Ui::PaletteWidget *ui;
};

#endif // PALETTEWIDGET_H
