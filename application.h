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
#include "session.h"

class Application : public QApplication, public QOpenGLFunctions_3_3_Core
{
public:
    explicit Application(int &argc, char **argv);
    ~Application();

    static const QString fileDialogFilterString;

    bool addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src);
    GLuint shader(const QString &name) { return m_shaders[name]->shaderId(); }
    bool addProgram(const QString &name, const QStringList &shaders);
    GLuint program(const QString &name) { return m_programs[name]->programId(); }
    QOpenGLWidget *shareWidget() { return m_shareWidget; }
    Session *session() { return m_session; }
    static const GLfloat brushVertices[][2];
    GLuint brushVertexBuffer;

private:
    GLuint processShader(const GLenum type, const QString &src);
    GLuint processProgram(const QStringList &shaders);
    QHash<QString, QOpenGLShader *> m_shaders;
    QHash<QString, QOpenGLShaderProgram *> m_programs;
    MainWindow *m_window;
    QOpenGLWidget *m_shareWidget;
    Session *m_session;
};


#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
