#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QQueue>
#include <QUndoStack>
#define GL_GLEXT_PROTOTYPES
#include <QOpenGLFunctions>

class QOpenGLFramebufferObject;

struct ImageDataFormatDefinition {
    char NAME[16];
    GLint INTERNAL_FORMAT;
    GLint FORMAT;
    GLint ENUM;
    GLint SIZE;
};

extern const ImageDataFormatDefinition IMAGE_DATA_FORMATS[];

enum class ImageDataFormat {
    Indexed,
    RGBA,
};

class ImageData : public QObject, public QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit ImageData(const QSize &size, ImageDataFormat format, const GLubyte *const data = nullptr);
    ~ImageData();
    uint pixel(const QPoint &position) const;
    void setPixel(const QPoint &position, uint colour);
private:
    QSize m_size;
    ImageDataFormat m_format;
    GLuint m_texture;
    GLuint m_framebuffer;
    GLuint m_vertexBuffer;
};

class ImageLayerCel : public QObject
{
    Q_OBJECT
private:
    qreal m_begin, m_end;
    ImageData *m_texture;
    // transform
};

class ImageLayer : public QObject
{
    Q_OBJECT
private:
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
    explicit Image(const Image &image, QObject *parent = nullptr);
    explicit Image(const QSize &size, Format format, QObject *parent = nullptr);
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


private:
    QString m_fileName;
    QImage m_data;
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
    QOpenGLFramebufferObject *m_fbo;
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

private:
    const QPoint &point0, &point1;
    const Image &image;
//    Image dirty;
};

#endif // IMAGE_H
