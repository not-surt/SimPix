#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "data.h"

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QObject *parent = nullptr);
    explicit Document(const QString &fileName, QObject *parent = nullptr);
    virtual ~Document() {}

//    bool reopen();
    bool save(QString fileName = QString());

    const QString &fileName() const;
    QString shortName() const;
    void setFileName(const QString &fileName);
    bool dirty() const;

signals:
    void fileNameChanged(const QString &fileName);
    void dirtied();
    void dirtyChanged();

public slots:

protected:
    void makeDirty();
    virtual bool doSave(QString fileName) = 0;

    QString m_fileName;
    bool m_dirty;
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

class Image : public Document
{
    Q_OBJECT
public:
    enum ContextColour {
        Primary,
        Secondary,
        Eraser,
    };
    explicit Image(const QSize &size, ImageDataFormat format, QObject *parent = nullptr);
    explicit Image(const QString &fileName, const char *format = nullptr, QObject *parent = nullptr);
    ~Image();

    ImageDataFormat format() const;

    ImageData *imageData();
    PaletteData *paletteData();
    QList<ImageEditor *> &editors();

signals:

public slots:
    void point(const QPoint &position, EditingContext *const editingContext);
    void stroke(const QPoint &a, const QPoint &b, EditingContext *const editingContext);
    void pick(const QPoint &position, EditingContext *const editingContext);

protected:
    virtual bool doSave(QString fileName);

    ImageData *m_imageData;
    PaletteData *m_paletteData;
    QList<ImageData *> m_layers;
    QList<ImageEditor *> m_editors;
};

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
