#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QQueue>
#include <QUndoStack>
#define GL_GLEXT_PROTOTYPES
#include <QOpenGLFunctions>

class QOpenGLFramebufferObject;

enum class ImageDataFormat {
    Indexed,
    RGBA,
    Invalid,
};

struct ImageDataFormatDefinition {
    ImageDataFormat id;
    char name[16];
    GLint internalFormat;
    GLint format;
    GLint glEnum;
    GLint size;
};

extern const ImageDataFormatDefinition IMAGE_DATA_FORMATS[];

union Pixel {
    GLubyte index;
    GLuint rgba;
    GLubyte bytes[4];
    struct {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte a;
    } components;
};

class TextureData : public QOpenGLFunctions
{
public:
    explicit TextureData(const QSize &size, const ImageDataFormat format, const GLubyte *const data = nullptr);
    ~TextureData();
    uint pixel(const QPoint &position);
    void setPixel(const QPoint &position, const uint colour);
    QSize size() const;
    ImageDataFormat format() const;
    GLuint texture() const;
    GLuint framebuffer() const;
    GLubyte *readData(GLubyte *const data = nullptr);
    void writeData(const GLubyte *const data);
protected:
    QSize m_size;
    ImageDataFormat m_format;
    GLuint m_texture;
    GLuint m_framebuffer;
};

class PaletteData : public TextureData
{
public:
    explicit PaletteData(const GLuint length, const GLubyte *const data = nullptr);
    uint colour(const uint index);
    void setColour(const uint index, uint colour);
    GLuint length() const;
};

class ImageData : public TextureData
{
public:
    explicit ImageData(const QSize &size, const ImageDataFormat format, const GLubyte *const data = nullptr);
    ~ImageData();
    GLuint vertexBuffer() const;
protected:
    GLuint m_vertexBuffer;
};

class ImageLayerCel : public QObject
{
    Q_OBJECT
protected:
    qreal m_begin, m_end;
    ImageData *m_texture;
    // transform
};

class ImageLayer : public QObject
{
    Q_OBJECT
protected:
    QList<ImageLayerCel> m_cels;
};

class Image : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QImage data READ data)
    Q_PROPERTY(Image::Format format READ format)
    Q_PROPERTY(uint contextColour READ contextColour WRITE setContextColour NOTIFY contextColourChanged)
    Q_PROPERTY(Image::ContextColour activeContextColour READ activeContextColour WRITE setActiveContextColour NOTIFY activeContextColourChanged)
    Q_PROPERTY(bool dirty READ dirty)
    Q_ENUMS(data format primaryColour secondaryColour)
public:
    enum Format {
        Invalid = QImage::Format_Invalid,
        Indexed = QImage::Format_Indexed8,
        RGBA = QImage::Format_ARGB32,
    };
    enum ContextColour {
        Primary,
        Secondary,
        Eraser,
    };
//    explicit Image(const Image &image, QObject *parent = nullptr);
//    explicit Image(const QSize &size, Format format, QObject *parent = nullptr);
    explicit Image(const QString &fileName, const char *format = nullptr, QObject *parent = nullptr);
    ~Image();

    QImage &data();
    const QImage &constData() const;

    Image::Format format() const;

    uint contextColour(const ContextColour contextColour = Primary) const;

    QUndoStack *undoStack = new QUndoStack(this);

    const QString &fileName() const;
    bool save(QString fileName = QString());

    bool dirty() const;
    bool isIndexed();

    ContextColour activeContextColour() const;

    ImageData *imageData();
    PaletteData *paletteData();

signals:
    void fileNameChanged(const QString &fileName);
    void changed(const QRegion &region);
    void contextColourChanged(const uint colour, const int context = Primary);

    void activeContextColourChanged(uint arg);

public slots:
    void point(const QPoint &position, const ContextColour contextColour = Primary);
    void stroke(const QPoint &a, const QPoint &b, const ContextColour contextColour = Primary);
    void pick(const QPoint &position, const ContextColour contextColour = Primary);
    void setContextColour(const uint colour, const ContextColour contextColour = Primary);
    void setActiveContextColour(const ContextColour contextColour);
    void setFileName(const QString &fileName);


protected:
    QString m_fileName;
    QImage m_data;
    ImageData *m_imageData;
    PaletteData *m_paletteData;
    QQueue<QPoint> recentPoints;
    static const int recentPointsMax = 5;
    void addRecentPoint(const QPoint &point)
    {
        recentPoints.enqueue(point);
        while (recentPoints.length() > recentPointsMax) {
            recentPoints.dequeue();
        }
    }
    uint m_primaryColour;
    uint m_secondaryColour;
    uint m_eraserColour;
    bool m_dirty;
    ContextColour m_activeContextColour;
};

Q_DECLARE_METATYPE(Image::Format)
Q_DECLARE_METATYPE(Image::ContextColour)


class StrokeCommand : public QUndoCommand
{
public:
    StrokeCommand(const Image &image, const QPoint &point0, const QPoint &point1, QUndoCommand *parent = nullptr);
    ~StrokeCommand();

    void undo();
    void redo();

protected:
    const QPoint &point0, &point1;
    const Image &image;
//    Image dirty;
};

#endif // IMAGE_H
