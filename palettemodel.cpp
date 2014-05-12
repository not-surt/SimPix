#include "palettemodel.h"

#include <QColor>
#include "application.h"

PaletteModel::PaletteModel(QObject *parent) :
    QAbstractListModel(parent), m_image(0)
{
}

int PaletteModel::rowCount(const QModelIndex &parent) const
{
    if (m_image && m_image->paletteData()) {
        return m_image->paletteData()->length();
    }
    else {
        return 0;
    }
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (!m_image || !m_image->paletteData() || (m_image && ((index.row() < 0) || (index.row() >= m_image->paletteData()->length())))) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        APP->contextMakeCurrent();
        QRgb colour = m_image->paletteData()->colour(index.row());
        return QColor(qRed(colour), qGreen(colour), qBlue(colour), qAlpha(colour));
    }
    else {
        return QVariant();
    }
}

QVariant PaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
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

Scene *PaletteModel::image() const
{
    return m_image;
}

void PaletteModel::setImage(Scene *image)
{
    beginResetModel();
    m_image = image;
    endResetModel();
}


bool PaletteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_image || !m_image->paletteData() || (m_image && ((index.row() < 0) || (index.row() >= m_image->paletteData()->length())))) {
        return false;
    }
    else {
        APP->contextMakeCurrent();
        m_image->paletteData()->setColour(index.row(), value.value<QColor>().rgba());
        emit dataChanged(index, index);
        return true;
    }
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index);
}
