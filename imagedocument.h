#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include "document.h"
#include "data.h"
#include "imageeditor.h"

class ImageDocument : public Document
{
    Q_OBJECT
public:
    enum ContextColour {
        Primary,
        Secondary,
        Eraser,
    };
    explicit ImageDocument(const QSize &size, TextureDataFormat format, QObject *parent = nullptr);
    explicit ImageDocument(const QString &fileName, const char *format = nullptr, QObject *parent = nullptr);
    ~ImageDocument();

    Editor *createEditor();
    static ImageDocument *newGui(QWidget *const parent = nullptr);
    static ImageDocument *openGui(QWidget *const parent = nullptr);
    void closeGui(QWidget *const parent = nullptr);
    bool saveGui(QWidget *const parent = nullptr);
    bool saveAsGui(QWidget *const parent = nullptr);

    TextureDataFormat format() const { return m_imageData->format(); }
    ImageData *imageData() { return m_imageData; }
    PaletteData *paletteData() { return m_paletteData; }

signals:

public slots:
    void point(const QPoint &position, EditingContext *const editingContext);
    void stroke(const QPoint &start, const QPoint &end, EditingContext *const editingContext);
    void pick(const QPoint &position, EditingContext *const editingContext);

protected:
    bool doOpen(QString fileName);
    bool doSave(QString fileName);

    ImageData *m_imageData;
    PaletteData *m_paletteData;
    QList<ImageData *> m_layers;
};

#endif // IMAGEDOCUMENT_H
