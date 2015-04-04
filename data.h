#ifndef DATA_H
#define DATA_H

#include <QSize>
#include <QQueue>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include "transform.h"
#include "editingcontext.h"

class ImageEditor;

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

class TextureData : public QOpenGLFunctions_3_3_Core
{
public:
    explicit TextureData(QOpenGLWidget *const widget, const QSize &size, const ImageDataFormat format, const GLubyte *const data = nullptr);
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
    QOpenGLWidget *m_widget;
    QSize m_size;
    ImageDataFormat m_format;
    GLuint m_texture;
    GLuint m_framebuffer;
};

class PaletteData : public TextureData
{
public:
    explicit PaletteData(QOpenGLWidget *const widget, const GLuint length, const GLubyte *const data = nullptr);
    uint colour(const uint index);
    void setColour(const uint index, uint colour);
    GLuint length() const;
};

class ImageData : public TextureData
{
public:
    explicit ImageData(QOpenGLWidget *const widget, const QSize &size, const ImageDataFormat format, const GLubyte *const data = nullptr);
    ~ImageData();
    GLuint vertexBuffer() const;
    const QRect &rect();
protected:
    GLuint m_vertexBuffer;
};

class ImageLayerCel
{
public:
    ImageLayerCel()
        : m_begin(0.f), m_end(0.f), m_imageData(nullptr), m_paletteData(nullptr)
    {

    }

protected:
    float m_begin, m_end;
    ImageData *m_imageData;
    PaletteData *m_paletteData;
};

class ImageLayer
{
public:
    ImageLayer()
        : m_cels(), transform(), m_paletteData(nullptr)
    {

    }

protected:
    QList<ImageLayerCel> m_cels;
    Transform transform;
    PaletteData *m_paletteData;
};

#endif // DATA_H
