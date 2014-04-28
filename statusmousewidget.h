#ifndef STATUSMOUSEWIDGET_H
#define STATUSMOUSEWIDGET_H

#include <QWidget>

namespace Ui {
class StatusMouseWidget;
}

class StatusMouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusMouseWidget(QWidget *parent = 0);
    ~StatusMouseWidget();

public slots:
    void setMouseInfo(const QPoint &position, const uint colour);

private:
    Ui::StatusMouseWidget *ui;
};

#endif // STATUSMOUSEWIDGET_H
