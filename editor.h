#ifndef EDITOR_H
#define EDITOR_H

#include "session.h"

class Document;

class Editor : public SessionItem
{
public:
    Document &document;

    explicit Editor(Document &document, const QString &name = QString());
    ~Editor();

    QString typeName() const {
        return "Editor";
    }
    TreeModelItem *child(int row) override {
        return nullptr;
    }
    int childCount() const override {
        return 0;
    }
    TreeModelItem *parent();
    int row();
};

#endif // EDITOR_H
