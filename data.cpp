#include "data.h"

#include <QDebug>
#include <QColor>
#include <QFileInfo>
#include "application.h"
#include "util.h"

const TextureDataFormatDefinition IMAGE_DATA_FORMATS[] = {
    {TextureDataFormat::Indexed, "Indexed", GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte), {GL_COLOR_ATTACHMENT0, GL_NONE}},
    {TextureDataFormat::RGBA, "RGBA", GL_RGBA8UI, GL_BGRA_INTEGER, GL_UNSIGNED_BYTE, sizeof(GLubyte) * 4, {GL_NONE, GL_COLOR_ATTACHMENT0}},
    {TextureDataFormat::Invalid, "", 0, 0, 0, 0}
};

TextureData::TextureData(const QSize &size, const TextureDataFormat _format, const GLubyte *const data) :
    OpenGLData(), size(size), format(_format),
    projectionMatrix([&]() {
        const float halfWidth = (float)size.width() / 2.f;
        const float halfHeight = (float)size.height() / 2.f;
        QMatrix4x4 temp;
        temp.scale(1.f / (float)halfWidth, 1.f / (float)halfHeight);
        temp.translate(-halfWidth, -halfHeight);
        return temp;} ()),
    texture([&](){
        const TextureDataFormatDefinition *const format = &IMAGE_DATA_FORMATS[(int)_format];
        GLuint texture;
        glGenTextures((GLsizei)1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, format->internalFormat, size.width(), size.height(), 0, format->format, format->glEnum, data);
        return texture; }()),
    framebuffer([&](){
        const TextureDataFormatDefinition *const format = &IMAGE_DATA_FORMATS[(int)_format];
        GLuint framebuffer;
        glGenFramebuffers((GLsizei)1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glDrawBuffers(2, format->buffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return framebuffer; }())
{
}

TextureData::~TextureData()
{
    GLContextGrabber grab(context, surface);

    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &framebuffer);
}

uint TextureData::pixel(const QPoint &position)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    uint colour = 0;
    glReadPixels(position.x(), position.y(), 1, 1, IMAGE_DATA_FORMATS[(int)format].format, IMAGE_DATA_FORMATS[(int)format].glEnum, &colour);
    return colour;
}

void TextureData::setPixel(const QPoint &position, const uint colour)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, position.x(), position.y(), 1, 1, IMAGE_DATA_FORMATS[(int)format].format, IMAGE_DATA_FORMATS[(int)format].glEnum, &colour);
}

GLubyte *TextureData::readData(GLubyte *const _data)
{
    uchar *data = (_data != nullptr) ? _data : new uchar[size.width() * size.height() * IMAGE_DATA_FORMATS[(int)format].size];
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glReadPixels(0, 0, size.width(), size.height(), IMAGE_DATA_FORMATS[(int)format].format, IMAGE_DATA_FORMATS[(int)format].glEnum, data);
    return data;
}

void TextureData::writeData(const GLubyte *const data)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.width(), size.height(), IMAGE_DATA_FORMATS[(int)format].format, IMAGE_DATA_FORMATS[(int)format].glEnum, data);
}

void TextureData::clear(const uint colour)
{
    static GLuint buffer[4];

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    buffer[0] = R(colour);
    buffer[1] = G(colour);
    buffer[2] = B(colour);
    buffer[3] = A(colour);
    glClearBufferuiv(GL_COLOR, 0, buffer);
}

PaletteData::PaletteData(const GLuint length, const GLubyte *const data) :
    TextureData(QSize(length, 1), TextureDataFormat::RGBA, data)
{
}

ImageData::ImageData(const QSize &size, const TextureDataFormat format, const GLubyte *const data) :
    TextureData(size, format, data), rect(QPoint(0, 0), size),
    vertexBuffer([&](){
        GLuint vertexBuffer;
        glGenBuffers((GLsizei)1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        const GLfloat vertices[][2] = {
            {0.f, 0.f},
            {(GLfloat)size.width(), 0.f},
            {(GLfloat)size.width(), (GLfloat)size.height()},
            {0.f, (GLfloat)size.height()},
        };
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
        return vertexBuffer; }())

{
}

ImageData::~ImageData()
{
    GLContextGrabber grab(context, surface);

    glDeleteBuffers(1, &vertexBuffer);
}
