#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "data.h"
#include <QFileInfo>

class Editor;

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QObject *parent = nullptr);
    explicit Document(const QString &fileName, QObject *parent = nullptr);
    virtual ~Document() {}

    bool revert() { return doOpen(m_fileName); }
    bool save(QString fileName = QString());

    const QString &fileName() const { return m_fileName; }
    QString shortName() const { return QFileInfo(m_fileName).fileName(); }
    void setFileName(const QString &fileName) {
        if (m_fileName != fileName) {
            m_fileName = fileName;
            emit fileNameChanged(fileName);
        }
    }
    bool dirty() const { return m_dirty; }
    virtual Editor *createEditor() = 0;
    void removeEditor(Editor *editor) { m_editors.remove(editor); }
    QSet<Editor *> &editors() { return m_editors; }

signals:
    void fileNameChanged(const QString &fileName);
    void dirtied();
    void dirtyChanged();

public slots:

protected:
    void makeDirty() {
        m_dirty = true;
        emit dirtied();
        emit dirtyChanged();
    }
    void clearDirty() {
        m_dirty = false;
        emit dirtyChanged();
    }

    virtual bool doOpen(QString fileName) = 0;
    virtual bool doSave(QString fileName) = 0;

    QString m_fileName;
    bool m_dirty;
    QSet<Editor *> m_editors;
};

//class Palette : public Document
//{
//    Q_OBJECT
//public:
//    explicit Palette(const uint &size, QObject *parent = nullptr);
//    explicit Palette(const QString &fileName, const char *format = nullptr, QObject *parent = nullptr);
//    ~Palette();

//    PaletteData *paletteData();

//protected:
//    PaletteData *m_paletteData;
//};

class DocumentFactory {
public:
    enum DocumentType {
        Image,
        Palette
    };
    bool create(const DocumentType type) {}
    bool open(QString fileName) {}
};

#endif // DOCUMENT_H
