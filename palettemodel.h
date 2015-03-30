#ifndef PALETTEMODEL_H
#define PALETTEMODEL_H

#include <QAbstractListModel>
#include "editingcontext.h"
#include <QDebug>

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

#endif // PALETTEMODEL_H
