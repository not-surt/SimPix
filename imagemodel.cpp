#include "imagemodel.h"

ImageModel::ImageModel(Image *image, QObject *parent) :
    QAbstractItemModel(parent), m_image(image)
{
}


QModelIndex ImageModel::index(int row, int column, const QModelIndex &parent) const
{
}

QModelIndex ImageModel::parent(const QModelIndex &child) const
{
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
}

int ImageModel::columnCount(const QModelIndex &parent) const
{
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
}

Image *ImageModel::image() const
{
    return m_image;
}

void ImageModel::setImage(Image *image)
{
    beginResetModel();
    m_image = image;
    endResetModel();
}
