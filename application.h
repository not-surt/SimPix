#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QOpenGLFunctions>

#include <QHash>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include "mainwindow.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

class Application : public QApplication, public QOpenGLFunctions
{
public:
    explicit Application(int &argc, char **argv);
    ~Application();
    void makeContextCurrent();
    QSurfaceFormat &format();
    QOpenGLContext &context();
//    bool addSampler();
    bool addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src);
    GLuint shader(const QString &name) const;
    bool addProgram(const QString &name, const QStringList &shaders);
    GLuint program(const QString &name) const;

private:
    GLuint processShader(const GLenum type, const QString &src);
    GLuint processProgram(const QStringList &shaders);
    QSurfaceFormat m_format;
    QOpenGLContext m_context;
    QOffscreenSurface m_offscreen;
    QHash<QString, GLuint> m_samplers;
    QHash<QString, QOpenGLShader *> m_shaders;
    QHash<QString, QOpenGLShaderProgram *> m_programs;
    MainWindow m_window;
};

#endif // APPLICATION_H
