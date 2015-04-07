#include "data.h"

#include <QDebug>
#include <QColor>
#include <QFileInfo>
#include "application.h"
#include "util.h"

const ImageDataFormatDefinition IMAGE_DATA_FORMATS[] = {
    {ImageDataFormat::Indexed, "Indexed", GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte)},
    {ImageDataFormat::RGBA, "RGBA", GL_RGBA8UI, GL_BGRA_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte) * 4},
    {ImageDataFormat::Invalid, "", 0, 0, 0, 0}
};

TextureData::TextureData(QOpenGLWidget *const widget, const QSize &size, const ImageDataFormat _format, const GLubyte *const data) :
    m_widget(widget), m_size(size), m_format(_format)
{
    const ImageDataFormatDefinition *const format = &IMAGE_DATA_FORMATS[(int)_format];

    ContextGrabber grab(m_widget);
    initializeOpenGLFunctions();

    glGenTextures((GLsizei)1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, format->internalFormat, size.width(), size.height(), 0, format->format, format->glEnum, data);

    glGenFramebuffers((GLsizei)1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
    const GLenum BUFFERS[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, BUFFERS);
    glViewport(0, 0, size.width(), size.height());
}

TextureData::~TextureData()
{
    ContextGrabber grab(m_widget);

    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_framebuffer);
}

uint TextureData::pixel(const QPoint &position)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    uint colour = 0;
    glReadPixels(position.x(), position.y(), 1, 1, IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, &colour);
    return colour;
}

void TextureData::setPixel(const QPoint &position, const uint colour)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, position.x(), position.y(), 1, 1, IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, &colour);
}

GLubyte *TextureData::readData(GLubyte *const _data)
{
    uchar *data = (_data != nullptr) ? _data : new uchar[m_size.width() * m_size.height() * IMAGE_DATA_FORMATS[(int)m_format].size];
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glReadPixels(0, 0, m_size.width(), m_size.height(), IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, data);
    return data;
}

void TextureData::writeData(const GLubyte *const data)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_size.width(), m_size.height(), IMAGE_DATA_FORMATS[(int)m_format].format, IMAGE_DATA_FORMATS[(int)m_format].glEnum, data);
}

void TextureData::clear(const uint colour)
{
    static GLuint buffer[4];

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
    buffer[0] = R(colour);
    buffer[1] = G(colour);
    buffer[2] = B(colour);
    buffer[3] = A(colour);
    glClearBufferuiv(GL_COLOR, 0, buffer);
}

PaletteData::PaletteData(QOpenGLWidget *const widget, const GLuint length, const GLubyte *const data) :
    TextureData(widget, QSize(length, 1), ImageDataFormat::RGBA, data)
{
}

ImageData::ImageData(QOpenGLWidget *const widget, const QSize &size, const ImageDataFormat format, const GLubyte *const data) :
    TextureData(widget, size, format, data)
{
    ContextGrabber grab(m_widget);

    glGenBuffers((GLsizei)1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    const GLfloat vertices[][2] = {
        {0.f, 0.f},
        {(GLfloat)size.width(), 0.f},
        {(GLfloat)size.width(), (GLfloat)size.height()},
        {0.f, (GLfloat)size.height()},
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
}

ImageData::~ImageData()
{
    ContextGrabber grab(m_widget);
    glDeleteBuffers(1, &m_vertexBuffer);
}

GLuint ImageData::vertexBuffer() const
{
    return m_vertexBuffer;
}

const QRect &ImageData::rect()
{
    return QRect(QPoint(0, 0), m_size);
}
