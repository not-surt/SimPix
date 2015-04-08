#ifndef EDITOR_H
#define EDITOR_H

class Document;

class Editor
{
public:
    explicit Editor(Document *document) :
        m_document(document) {}
    Document *document() { return m_document; }
protected:
    Document *m_document;
};

#endif // EDITOR_H
