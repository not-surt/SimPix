#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QOpenGLFunctions_3_3_Core>

#include <QHash>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include "mainwindow.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

class RenderingContext {
public:
    explicit RenderingContext(QSurfaceFormat *const format, RenderingContext *const shareContext = nullptr);
    QOffscreenSurface &surface();
    QOpenGLContext &context();

private:
    QOffscreenSurface m_surface;
    QOpenGLContext m_context;
};

class Application : public QApplication, public QOpenGLFunctions_3_3_Core
{
public:
    explicit Application(int &argc, char **argv);
    ~Application();
    void contextMakeCurrent();
    void contextDoneCurrent();
    QSurfaceFormat *format();
    QOpenGLContext *context();
//    bool addSampler();
    bool addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src);
    GLuint shader(const QString &name);
    bool addProgram(const QString &name, const QStringList &shaders);
    GLuint program(const QString &name);

private:
    GLuint processShader(const GLenum type, const QString &src);
    GLuint processProgram(const QStringList &shaders);
    QSurfaceFormat m_format;
    QOpenGLContext m_context;
    QOffscreenSurface m_offscreen;
//    RenderingContext m_renderingContext;
    QHash<QString, GLuint> m_samplers;
    QHash<QString, QOpenGLShader *> m_shaders;
    QHash<QString, QOpenGLShaderProgram *> m_programs;
    MainWindow m_window;
};

#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
