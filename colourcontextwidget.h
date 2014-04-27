#ifndef COLOURCONTEXTWIDGET_H
#define COLOURCONTEXTWIDGET_H

#include <QWidget>
#include "image.h"

namespace Ui {
class ColourContextWidget;
}

class ColourContextWidget : public QWidget
{
    Q_OBJECT
//    Q_PROPERTY(uint contextColour WRITE setContextColour NOTIFY contextColourChanged) // invalid

public:
    explicit ColourContextWidget(QWidget *parent = nullptr);
    ~ColourContextWidget();

public slots:
    void setContextColour(const uint colour, const int context = Image::Primary);

signals:
    void contextColourChanged(const uint colour, const int context = Image::Primary);

private:
    Ui::ColourContextWidget *ui;
};

#endif // COLOURCONTEXTWIDGET_H
