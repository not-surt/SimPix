#ifndef STATUSMOUSEWIDGET_H
#define STATUSMOUSEWIDGET_H

#include <QWidget>
#include "data.h"

namespace Ui {
class StatusMouseWidget;
}

class StatusMouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusMouseWidget(QWidget *parent = nullptr);
    ~StatusMouseWidget();

public slots:
    void setMouseInfo(const QPoint &position, const Colour &colour);

private:
    Ui::StatusMouseWidget *ui;
};

#endif // STATUSMOUSEWIDGET_H
