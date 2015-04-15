#ifndef SESSIONWIDGET_H
#define SESSIONWIDGET_H

#include <QListView>
#include <QStringListModel>
#include <QWidget>

class SessionModel : public QStringListModel
{
    Q_OBJECT
public:
    explicit SessionModel(QObject *parent = 0)
        : QStringListModel(parent) {}
};

class SessionView : public QListView
{
    Q_OBJECT
public:
    explicit SessionView(QWidget *parent = 0)
        : QListView(parent) {
        setModel(new SessionModel);
    }
};

namespace Ui {
class SessionWidget;
}

class SessionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SessionWidget(QWidget *parent = 0);
    ~SessionWidget();
    void setStringList(const QStringList &list);

signals:

public slots:
private:
    Ui::SessionWidget *ui;
};

#endif // SESSIONWIDGET_H
