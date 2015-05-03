#ifndef SESSION_H
#define SESSION_H

#include <QList>
#include <QAbstractItemModel>
#include "treemodel.h"

class SessionItem : public TreeModel::Item
{
public:
    QString name;

    explicit SessionItem(const QString &name = QString());
    virtual ~SessionItem();
    virtual QString typeName() const = 0;
    QVariant data(int column) const;
};

class Document;

class Session : public SessionItem, public TreeModel
{
public:
    QList<Document *> documents;

    explicit Session(const QString &name = QString());
    QString typeName() const;
    TreeModel::Item *child(int row) override;
    int childCount() const override;
    TreeModel::Item *parent();
    int row();
};

#endif // SESSION_H
