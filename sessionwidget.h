#ifndef SESSIONWIDGET_H
#define SESSIONWIDGET_H

#include <QTreeView>
#include <QMainWindow>
#include <QDebug>
#include "session.h"
#include "treemodel.h"

namespace Ui {
class SessionWidget;
}

class SessionWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit SessionWidget(QWidget *parent = 0);
    ~SessionWidget();
    void setSession(Session *const session);

signals:

public slots:
private:
    Ui::SessionWidget *ui;
    QTreeView &treeView;
};

#endif // SESSIONWIDGET_H
