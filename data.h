#ifndef DATA_H
#define DATA_H

#include <QSize>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>

class ImageEditor;

struct TextureDataFormat
{
    enum class Id {
        Indexed,
        RGBA,
        Invalid,
    };
    Id id;
    char name[16];
    GLint internalFormat;
    GLint format;
    GLint glEnum;
    GLint size;
    GLenum buffers[2];
};

extern const TextureDataFormat TEXTURE_DATA_FORMATS[];

#define B(uint) (uint & 0x000000ff)
#define G(uint) ((uint & 0x0000ff00) >> 8)
#define R(uint) ((uint & 0x00ff0000) >> 16)
#define A(uint) ((uint & 0xff000000) >> 24)

struct Colour {
    union {
        uint rgba;
        uchar bytes[4];
        struct {
            uchar b;
            uchar g;
            uchar r;
            uchar a;
        } components;
    };
    short index;

    explicit Colour(const uint p_rgba = 0, const int p_index = -1)
        : rgba(p_rgba), index(p_index) {}
    Colour(const Colour &colour)
        : Colour(colour.rgba, colour.index) {}
    inline bool operator==(const Colour &rhs) { return this->rgba == rhs.rgba && this->index == rhs.index; }
    inline bool operator!=(const Colour &rhs) { return !this->operator==(rhs); }
};

class OpenGLData : public QOpenGLFunctions_3_3_Core
{
public:
    QOpenGLContext *const context;
    QSurface *const surface;

    explicit OpenGLData()
        : context(QOpenGLContext::currentContext()), surface(context ? context->surface() : nullptr)
    {
        initializeOpenGLFunctions();
    }
};

class TextureData : public OpenGLData
{
public:
    const QSize size;
    const TextureDataFormat::Id format;
    const QMatrix4x4 projectionMatrix;
    const GLuint texture;
    const GLuint framebuffer;

    explicit TextureData(const QSize &size, const TextureDataFormat::Id format, const GLubyte *const data = nullptr);
    ~TextureData();
    uint pixel(const QPoint &position);
    void setPixel(const QPoint &position, const uint colour);
    GLubyte *readData(GLubyte *const data = nullptr);
    void writeData(const GLubyte *const data);
    void clear(const uint colour);
};

class PaletteData : public TextureData
{
public:
    explicit PaletteData(const GLuint length, const GLubyte *const data = nullptr);
    uint colour(const uint index) { return pixel(QPoint(index, 0)); }
    void setColour(const uint index, uint colour) { setPixel(QPoint(index, 0), colour); }
    GLuint length() const { return size.width(); }
};

class ImageData : public TextureData
{
public:
    const QRect rect;
    const GLuint vertexBuffer;

    explicit ImageData(const QSize &size, const TextureDataFormat::Id format, const GLubyte *const data = nullptr);
    ~ImageData();
};

//class ImageLayerCel
//{
//public:
//    ImageLayerCel()
//        : m_begin(0.f), m_end(0.f), m_imageData(nullptr), m_paletteData(nullptr) {}

//protected:
//    float m_begin, m_end;
//    ImageData *m_imageData;
//    PaletteData *m_paletteData;
//};

//class ImageLayer
//{
//public:
//    ImageLayer()
//        : m_cels(), transform(), m_paletteData(nullptr) {}

//protected:
//    QList<ImageLayerCel> m_cels;
//    Transform transform;
//    PaletteData *m_paletteData;
//};

#endif // DATA_H
