#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDebug>

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    class Item
    {
    public:
        explicit Item() {}
        virtual ~Item() {}

        virtual Item *child(int row) = 0;
        virtual int childCount() const = 0;
        virtual Item *parent() = 0;
        virtual int row() = 0;
        virtual QVariant data(int column) const = 0;
    };

    explicit TreeModel(QObject *parent = 0);
    ~TreeModel();
    void setRoot(Item *const root) { this->root = root; emit layoutChanged(); }
    void setHeadings(const QStringList headings) { this->headings = headings; }

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    Item *root;
    QStringList headings;
};

#endif // TREEMODEL_H
