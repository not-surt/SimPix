#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractItemModel>
#include "image.h"

class ImageModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage)
    Q_ENUMS(image)
public:
    explicit ImageModel(Image *image = nullptr, QObject *parent = nullptr);
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    Image *image() const;

signals:

public slots:
    void setImage(Image *image);

private:
    Image *m_image;
};

#endif // IMAGEMODEL_H
