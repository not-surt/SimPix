#include "data.h"

#include <QDebug>
#include <QColor>
#include <QFileInfo>
#include "application.h"
#include "util.h"

const TextureDataFormatDefinition IMAGE_DATA_FORMATS[] = {
    {TextureDataFormat::Indexed, "Indexed", GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte)},
    {TextureDataFormat::RGBA, "RGBA", GL_RGBA8UI, GL_BGRA_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte) * 4},
    {TextureDataFormat::Invalid, "", 0, 0, 0, 0}
};

TextureData::TextureData(QOpenGLWidget *const widget, const QSize &size, const TextureDataFormat _format, const GLubyte *const data) :
    OpenGLData(), m_widget(widget), m_size(size), m_format(_format),
    matrix([&](){
        const float halfWidth = (float)size.width() / 2.f;
        const float halfHeight = (float)size.height() / 2.f;
        QMatrix4x4 temp;
        temp.scale(1.f / (float)halfWidth, 1.f / (float)halfHeight);
        temp.translate(-halfWidth, -halfHeight);
        return temp;}())
{
    const TextureDataFormatDefinition *const format = &IMAGE_DATA_FORMATS[(int)_format];

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
    if (format->id == TextureDataFormat::Indexed) {
        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_NONE};
        glDrawBuffers(2, buffers);
    }
    else {
        GLenum buffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0};
        glDrawBuffers(2, buffers);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    TextureData(widget, QSize(length, 1), TextureDataFormat::RGBA, data)
{
}

ImageData::ImageData(QOpenGLWidget *const widget, const QSize &size, const TextureDataFormat format, const GLubyte *const data) :
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
