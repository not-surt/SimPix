#ifndef COLOURCONTEXTWIDGET_H
#define COLOURCONTEXTWIDGET_H

#include <QWidget>

namespace Ui {
class ColourContextWidget;
}

class ColourContextWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColourContextWidget(QWidget *parent = 0);
    ~ColourContextWidget();

private:
    Ui::ColourContextWidget *ui;
};

#endif // COLOURCONTEXTWIDGET_H
