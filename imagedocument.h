#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include "document.h"
#include "data.h"
#include "imageeditor.h"

class ImageDocument : public Document
{
    Q_OBJECT
public:
    explicit ImageDocument(const QSize &size, TextureData::Format::Id format, QObject *parent = nullptr);
    explicit ImageDocument(const QString &fileName, const char *format = nullptr, QObject *parent = nullptr);
    ~ImageDocument();
    Colour pixel(const QPoint &position);
    void setPixel(const QPoint &position, const Colour &colour);
    void clear(const Colour &colour);

    Editor *createEditor();
    static ImageDocument *newGui(QWidget *const parent = nullptr);
    static ImageDocument *openGui(QWidget *const parent = nullptr);
    void closeGui(QWidget *const parent = nullptr);
    bool saveGui(QWidget *const parent = nullptr);
    bool saveAsGui(QWidget *const parent = nullptr);

    TextureData::Format::Id format() const { return m_imageData->format; }
    ImageData *imageData() { return m_imageData; }
    PaletteData *paletteData() { return m_paletteData; }

protected:
    bool doOpen(QString fileName);
    bool doSave(QString fileName);

    ImageData *m_imageData;
    PaletteData *m_paletteData;
    QList<ImageData *> m_layers;
};

#endif // IMAGEDOCUMENT_H
