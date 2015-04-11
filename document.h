#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "data.h"
#include <QFileInfo>

class Editor;

class FileInfo : public QObject
{
    Q_OBJECT
public:
    explicit FileInfo(const QString &fileName = QString(), QObject *parent = nullptr) :
        QObject(parent), m_fileName(fileName), m_dirty(false) {}
    const QString &fileName() const { return m_fileName; }
    QString shortName() const { return QFileInfo(m_fileName).fileName(); }
    bool dirty() const { return m_dirty; }

signals:
    void fileNameChanged(const QString &fileName);
    void dirtied();
    void dirtyChanged();

public slots:
    void setFileName(const QString &fileName) {
        if (m_fileName != fileName) {
            m_fileName = fileName;
            emit fileNameChanged(fileName);
        }
    }
    void makeDirty() {
        m_dirty = true;
        emit dirtied();
        emit dirtyChanged();
    }
    void clearDirty() {
        m_dirty = false;
        emit dirtyChanged();
    }

protected:
    QString m_fileName;
    bool m_dirty;
};

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(const QString &fileName = QString(), QObject *parent = nullptr);
    virtual ~Document() {}

    bool revert() { return doOpen(fileInfo.fileName()); }
    bool save(QString fileName = QString());
    virtual Editor *createEditor() = 0;


    QSet<Editor *> editors;
    FileInfo fileInfo;

signals:

public slots:

protected:
    virtual bool doOpen(QString fileName) = 0;
    virtual bool doSave(QString fileName) = 0;
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

struct DocumentType {
    char *id;
    char **extensions;
    Document *(*create)();
    Document *(*open)(const QString &fileName);
};

extern const struct DocumentType DOCUMENT_TYPES[];

class DocumentFactory {
public:
    enum DocumentType {
        Image,
        Palette
    };
    bool create(const DocumentType type) { Q_UNUSED(type); }
    bool open(QString fileName) { Q_UNUSED(fileName); }
    QMap<QString, Document *(*)()> creators;
private:
};

#endif // DOCUMENT_H
