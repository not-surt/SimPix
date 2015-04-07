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

#define B(uint) (uint & 0x000000ff)
#define G(uint) ((uint & 0x0000ff00) >> 8)
#define R(uint) ((uint & 0x00ff0000) >> 16)
#define A(uint) ((uint & 0xff000000) >> 24)

class TextureData : public QOpenGLFunctions_3_3_Core
{
public:
    explicit TextureData(QOpenGLWidget *const widget, const QSize &size, const ImageDataFormat format, const GLubyte *const data = nullptr);
    ~TextureData();
    uint pixel(const QPoint &position);
    void setPixel(const QPoint &position, const uint colour);
    QSize size() const { return m_size; }
    ImageDataFormat format() const { return m_format; }
    GLuint texture() const { return m_texture; }
    GLuint framebuffer() const { return m_framebuffer; }
    GLubyte *readData(GLubyte *const data = nullptr);
    void writeData(const GLubyte *const data);
    void clear(const uint colour);
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
    uint colour(const uint index) { return pixel(QPoint(index, 0)); }
    void setColour(const uint index, uint colour) { setPixel(QPoint(index, 0), colour); }
    GLuint length() const { return m_size.width(); }
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
        : m_begin(0.f), m_end(0.f), m_imageData(nullptr), m_paletteData(nullptr) {}

protected:
    float m_begin, m_end;
    ImageData *m_imageData;
    PaletteData *m_paletteData;
};

class ImageLayer
{
public:
    ImageLayer()
        : m_cels(), transform(), m_paletteData(nullptr) {}

protected:
    QList<ImageLayerCel> m_cels;
    Transform transform;
    PaletteData *m_paletteData;
};

#endif // DATA_H
