#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QAbstractListModel>
#include "editingcontext.h"
#include <QDebug>
#include <QListView>
#include <QStyledItemDelegate>
#include "editingcontext.h"
#include <QWidget>
#include <QColor>
#include "editingcontext.h"

class PaletteModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PaletteModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

    EditingContext *editingContext() const;

signals:

public slots:
    void setEditingContext(EditingContext *editingContext);


private:
    EditingContext *m_editingContext;


    // QAbstractItemModel interface
public:
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
};

class PaletteView : public QListView
{
    Q_OBJECT
public:
    explicit PaletteView(QWidget *parent = nullptr);
    EditingContext *editingContext() const;

signals:

public slots:
    void setEditingContext(EditingContext *editingContext);


    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
};

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
    void colourChanged();

public slots:
    void setEditingContext(EditingContext *const editingContext);
    void editColour(QModelIndex index);
    void setColour(QModelIndex index);

private:
    Ui::PaletteWidget *ui;
};

#endif // PALETTEWIDGET_H
