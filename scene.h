#ifndef SCENE_H
#define SCENE_H

#include <QSize>
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
    const QRect &rect();
protected:
    GLuint m_vertexBuffer;
};

class ImageLayerCel : public QObject
{
    Q_OBJECT
protected:
    qreal m_begin, m_end;
    ImageData *m_imageData;
    PaletteData *m_paletteData;
    // transform
};

class ImageLayer : public QObject
{
    Q_OBJECT
protected:
    QList<ImageLayerCel> m_cels;
};

class Scene : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(ImageDataFormat format READ format)
    Q_PROPERTY(uint contextColour READ contextColour WRITE setContextColour NOTIFY contextColourChanged)
    Q_PROPERTY(bool dirty READ dirty)
    Q_ENUMS(data format primaryColour secondaryColour)
public:
    enum ContextColour {
        Primary,
        Secondary,
        Eraser,
    };
//    explicit Image(const Image &image, QObject *parent = nullptr);
    explicit Scene(const QSize &size, ImageDataFormat format, QObject *parent = nullptr);
    explicit Scene(const QString &fileName, const char *format = nullptr, QObject *parent = nullptr);
    ~Scene();

    ImageDataFormat format() const;

    uint contextColour(const ContextColour contextColour = Primary) const;

    QUndoStack *undoStack = new QUndoStack(this);

    const QString &fileName() const;
    bool save(QString fileName = QString());

    bool dirty() const;

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
    ImageData *m_imageData;
    PaletteData *m_paletteData;
//    QQueue<QPoint> recentPoints;
//    static const int recentPointsMax = 5;
//    void addRecentPoint(const QPoint &point)
//    {
//        recentPoints.enqueue(point);
//        while (recentPoints.length() > recentPointsMax) {
//            recentPoints.dequeue();
//        }
//    }
    uint m_contextColours[3];
    bool m_dirty;
    ContextColour m_activeContextColour;
};


class StrokeCommand : public QUndoCommand
{
public:
    StrokeCommand(const Scene &image, const QPoint &point0, const QPoint &point1, QUndoCommand *parent = nullptr);
    ~StrokeCommand();

    void undo();
    void redo();

protected:
    const QPoint &point0, &point1;
    const Scene &image;
//    Image dirty;
};

#endif // SCENE_H
