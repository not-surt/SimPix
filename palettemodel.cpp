#include "palettemodel.h"

#include <QColor>
#include "application.h"
#include "util.h"

PaletteModel::PaletteModel(QObject *parent) :
    QAbstractListModel(parent), m_editingContext(nullptr)
{
}

int PaletteModel::rowCount(const QModelIndex &parent) const
{
    if (m_editingContext && m_editingContext->palette()) {
        return m_editingContext->palette()->length();
    }
    else {
        return 0;
    }
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "1:" << m_editingContext;
    if (!index.isValid()) {
        return QVariant();
    }

    if (!m_editingContext || !m_editingContext->palette() || (m_editingContext && ((index.row() < 0) || (index.row() >= (int)m_editingContext->palette()->length())))) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
//        qDebug() << "2:" << m_editingContext << m_editingContext->palette();
        QRgb colour;
        GLContextGrabber grab(APP->shareWidget());
        colour = m_editingContext->palette()->colour(index.row());
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

EditingContext *PaletteModel::editingContext() const
{
    return m_editingContext;
}

void PaletteModel::setEditingContext(EditingContext *editingContext)
{
    beginResetModel();
    m_editingContext = editingContext;
    endResetModel();
}


bool PaletteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_editingContext || !m_editingContext->palette() || (m_editingContext && ((index.row() < 0) || (index.row() >= (int)m_editingContext->palette()->length())))) {
        return false;
    }
    else {
        GLContextGrabber grab(APP->shareWidget());
        m_editingContext->palette()->setColour(index.row(), value.value<QColor>().rgba());
        emit dataChanged(index, index);
        return true;
    }
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index);
}
