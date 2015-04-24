#include "editor.h"

#include "document.h"

Editor::Editor(Document &document, const QString &name)
    : SessionItem(name), document(document)
{
    document.editors.append(this);
}

Editor::~Editor()
{
    document.editors.removeOne(this);
}

TreeModelItem *Editor::parent() {
    return &document;
}

int Editor::row() {
    return document.editors.indexOf(this);
}
