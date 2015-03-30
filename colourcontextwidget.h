#ifndef COLOURCONTEXTWIDGET_H
#define COLOURCONTEXTWIDGET_H

#include <QWidget>
#include "scene.h"

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
    void setContextColour(const uint colour, const int context = Scene::Primary);

signals:
    void contextColourChanged(const uint colour, const int context = Scene::Primary);

private:
    Ui::ColourContextWidget *ui;
};

#endif // COLOURCONTEXTWIDGET_H
