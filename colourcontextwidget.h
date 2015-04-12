#ifndef COLOURCONTEXTWIDGET_H
#define COLOURCONTEXTWIDGET_H

#include <QWidget>
#include "document.h"
#include "editingcontext.h"

namespace Ui {
class ColourContextWidget;
}

class ColourContextWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColourContextWidget(QWidget *parent = nullptr);
    ~ColourContextWidget();

public slots:
    void setColourSlot(const EditingContext::ColourSlot colour, const EditingContext::ColourSlotId slot = EditingContext::ColourSlotId::Primary);

signals:
    void contextColourChanged(const EditingContext::ColourSlot colour, const EditingContext::ColourSlotId slot = EditingContext::ColourSlotId::Primary);

private:
    Ui::ColourContextWidget *ui;
};

#endif // COLOURCONTEXTWIDGET_H
