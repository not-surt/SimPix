#ifndef DATA_H
#define DATA_H

#include <QSize>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>

struct Rgba {
    union {
        GLuint rgba;
        GLubyte bytes[4];
        struct {
            GLubyte b;
            GLubyte g;
            GLubyte r;
            GLubyte a;
        };
    };

    explicit Rgba(const GLuint rgba = 0x00000000)
        : rgba(rgba) {}
    explicit Rgba(const GLubyte *const bytes) { if (bytes) { memcpy(this->bytes, bytes, 4 * sizeof(GLubyte)); } }
    explicit Rgba(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte a)
        : b(b), g(g), r(r), a(a) {}
    Rgba(const Rgba &rgba)
        : Rgba(rgba.rgba) {}
    inline bool operator==(const Rgba &rhs) { return this->rgba == rhs.rgba; }
    inline bool operator!=(const Rgba &rhs) { return !this->operator==(rhs); }
};

struct Colour : Rgba {
    GLshort index;

    explicit Colour(const GLuint rgba = 0x00000000, const GLshort index = -1)
        : Rgba(rgba), index(index) {}
    explicit Colour(const Rgba rgba, const GLshort index = -1)
        : Rgba(rgba), index(index) {}
    explicit Colour(const GLubyte *const bytes, const GLshort index = -1)
        : Rgba(bytes), index(index) {}
    explicit Colour(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte a, const GLshort index = -1)
        : Rgba(r, g, b, a), index(index) {}
    Colour(const Colour &colour)
        : Colour(colour.rgba, colour.index) {}
    inline bool operator==(const Colour &rhs) { return this->Rgba::operator==(rhs) && this->index == rhs.index; }
    inline bool operator!=(const Colour &rhs) { return !this->operator==(rhs); }
};

class OpenGLData : public QOpenGLFunctions_3_3_Core
{
public:
    QOpenGLContext *const context;
    QSurface *const surface;

    explicit OpenGLData()
        : context(QOpenGLContext::currentContext()), surface(context ? context->surface() : nullptr) { initializeOpenGLFunctions(); }
};

class TextureData : public OpenGLData
{
public:
    struct Format {
        enum Id {
            Indexed,
            RGBA,
        };
        Id id;
        char *name;
        GLint internalFormat;
        GLint format;
        GLint glEnum;
        GLint size;
        GLenum buffers[2];
        GLint outputBuffer;
    };
    static const Format FORMATS[2];

    const QSize size;
    const Format::Id format;
    const GLuint texture;
    const GLuint framebuffer;

    explicit TextureData(const QSize &size, const Format::Id format, const GLubyte *const data = nullptr);
    ~TextureData();
    GLubyte *readPixel(const QPoint &position, GLubyte *const buffer = nullptr);
    void writePixel(const QPoint &position, const GLubyte *const data);
    GLubyte *readData(GLubyte *const buffer = nullptr);
    void writeData(const GLubyte *const data);
    void clear(const GLubyte *const data);
};

class PaletteData : public TextureData
{
public:
    explicit PaletteData(const GLuint length, const GLubyte *const data = nullptr);
    Rgba colour(const uint index) { Rgba rgba; readPixel(QPoint(index, 0), rgba.bytes); return rgba; }
    void setColour(const uint index, const Rgba &rgba) { writePixel(QPoint(index, 0), rgba.bytes); }
    GLuint length() const { return size.width(); }
};

class ImageData : public TextureData
{
public:
    const QRect rect;
    const QMatrix4x4 projectionMatrix;
    const GLuint vertexBuffer;

    explicit ImageData(const QSize &size, const Format::Id format, const GLubyte *const data = nullptr);
    ~ImageData();
};

#endif // DATA_H
