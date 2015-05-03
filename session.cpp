#include "session.h"

#include "document.h"

SessionItem::SessionItem(const QString &name)
    : name(name) {}

SessionItem::~SessionItem() {}

QVariant SessionItem::data(int column) const{
    if (column == 0) {
        return typeName();
    }
    else if (column == 1) {
        return name;
    }
    else {
        return QVariant();
    }
}

Session::Session(const QString &name)
    : SessionItem(name), TreeModel(), documents() {
    setRoot(this);
    setHeadings({"Type", "Name", "File Name", "Editors"});
}

QString Session::typeName() const {
    return "Session";
}

TreeModel::Item *Session::child(int row) {
    return documents.value(row);
}

int Session::childCount() const {
    return documents.count();
}

TreeModel::Item *Session::parent() {
    return nullptr;
}

int Session::row() {
    return 0;
}
