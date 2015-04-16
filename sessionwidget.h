#ifndef SESSIONWIDGET_H
#define SESSIONWIDGET_H

#include <QListView>
#include <QAbstractListModel>
#include <QMainWindow>

class SessionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SessionModel(QObject *parent = 0)
        : QAbstractListModel(parent) {}
    int rowCount(const QModelIndex &parent) const {
        return list.length();
    }
    QVariant data(const QModelIndex &index, int role) const {
        if (!index.isValid()) {
            return QVariant();
        }
        else if (index.row() < 0 || index.row() >= list.length()) {
            return QVariant();
        }
        else if (role == Qt::DisplayRole) {
            return list[index.row()];
        }
        else {
            return QVariant();
        }
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const {
        if (role != Qt::DisplayRole) {
            return QVariant();
        }

        if (orientation == Qt::Horizontal) {
            return QString("Column %1").arg(section);
        }
        else {
            return QString("Row %1").arg(section);
        }
    }
    void setList(const QStringList &p_list) {
        list = p_list;
        emit layoutChanged();
    }

    QStringList list;
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

class SessionWidget : public QMainWindow
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
