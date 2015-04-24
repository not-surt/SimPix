#ifndef SESSION_H
#define SESSION_H

#include <QList>
#include "treemodel.h"

class SessionItem : public TreeModelItem
{
public:
    explicit SessionItem(const QString &name = QString())
        : m_name(name) {}
    virtual ~SessionItem() {}
    virtual QString typeName() const = 0;
    QString name() const {
        return m_name;
    }
    QVariant data(int column) const{
        if (column == 0) {
            return typeName();
        }
        else if (column == 1) {
            return name();
        }
        else {
            return QVariant();
        }
    }

protected:
    QString m_name;
};

class Document;

class Session : public SessionItem
{
public:
    QList<QWidget *> windows;
    QList<Document *> documents;

    explicit Session(const QString &name = QString())
        : SessionItem(name), windows(), documents() {}
    QString typeName() const {
        return "Session";
    }
    TreeModelItem *child(int row) override;
    int childCount() const override {
        return documents.count();
    }
    TreeModelItem *parent() {
        return nullptr;
    }
    int row() {
        return 0;
    }
};

#endif // SESSION_H
