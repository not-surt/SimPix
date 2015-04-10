#ifndef EDITOR_H
#define EDITOR_H

class Document;

class Editor
{
public:
    explicit Editor(Document &_document) :
        document(_document) {}
    Document &document;
protected:
};

#endif // EDITOR_H
