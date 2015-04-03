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
//    void openImage(const QString &path);
//    bool removeImage(const QString &path);
//    Image *image(const QString &path);

private:
    GLuint processShader(const GLenum type, const QString &src);
    GLuint processProgram(const QStringList &shaders);
    QOpenGLWidget *m_shareWidget;
    QHash<QString, QOpenGLShader *> m_shaders;
    QHash<QString, QOpenGLShaderProgram *> m_programs;
    MainWindow m_window;
//    QHash<QString, Image *> m_images;
};

#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
