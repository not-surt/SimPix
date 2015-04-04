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
#include <QOpenGLWidget>

class Application : public QApplication, public QOpenGLFunctions_3_3_Core
{
public:
    explicit Application(int &argc, char **argv);
    ~Application();
    bool addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src);
    GLuint shader(const QString &name);
    bool addProgram(const QString &name, const QStringList &shaders);
    GLuint program(const QString &name);
    QOpenGLWidget *shareWidget();

private:
    GLuint processShader(const GLenum type, const QString &src);
    GLuint processProgram(const QStringList &shaders);
    QHash<QString, QOpenGLShader *> m_shaders;
    QHash<QString, QOpenGLShaderProgram *> m_programs;
    MainWindow *m_window;
    QOpenGLWidget *m_shareWidget;
};

#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
