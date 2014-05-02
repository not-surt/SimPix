#ifndef PALETTEMODEL_H
#define PALETTEMODEL_H

#include <QAbstractListModel>
#include "image.h"
#include <QDebug>

class PaletteModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage)
    Q_ENUMS(image)
public:
    explicit PaletteModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

    Image *image() const;

signals:

public slots:
    void setImage(Image *image);


private:
    Image *m_image;


    // QAbstractItemModel interface
public:
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // PALETTEMODEL_H
