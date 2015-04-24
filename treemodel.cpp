#include "treemodel.h"

#include <QStringList>
#include <QDebug>

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent), root(nullptr)
{
}

TreeModel::~TreeModel()
{
    delete root;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return headings.length();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeModelItem *item = static_cast<TreeModelItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headings[section];

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if (!root)
        return QModelIndex();

    if (!hasIndex(row, column, parentIndex))
        return QModelIndex();

    TreeModelItem *parent;

    if (!parentIndex.isValid())
        parent = root;
    else
        parent = static_cast<TreeModelItem*>(parentIndex.internalPointer());

    TreeModelItem *child = parent->child(row);
    if (child)
        return createIndex(row, column, child);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!root)
        return QModelIndex();

    if (!index.isValid())
        return QModelIndex();

    TreeModelItem *child = static_cast<TreeModelItem *>(index.internalPointer());
    TreeModelItem *parent = child->parent();

    if (parent == root)
        return QModelIndex();

    return createIndex(parent->row(), 0, parent);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeModelItem *parentItem;

    if (!root)
        return 0;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<TreeModelItem*>(parent.internalPointer());

    return parentItem->childCount();
}
