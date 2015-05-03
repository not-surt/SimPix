#include "editor.h"

#include "document.h"

Editor::Editor(Document &document, const QString &name)
    : SessionItem(name), document(document)
{
    document.editors.append(this);
    emit document.session.layoutChanged();
}

Editor::~Editor()
{
    document.editors.removeOne(this);
    emit document.session.layoutChanged();
}

TreeModel::Item *Editor::parent() {
    return &document;
}

int Editor::row() {
    return document.editors.indexOf(this);
}
