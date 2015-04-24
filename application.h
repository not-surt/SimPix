#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QOpenGLFunctions_3_3_Core>
#include <QSettings>

#include <QHash>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include "mainwindow.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QKeySequence>
#include "session.h"

class Application : public QApplication, public QOpenGLFunctions_3_3_Core
{
public:
    static const QString fileDialogFilterString;
    static const GLfloat brushVertices[][2];
    struct ActionDefinition {
        char *name;
        char *text;
        char *icon;
        int standardShortcut;
        char *customShortcut;
        char *iconText;
        char *toolTip;
        char *statusTip;
    };
    static const std::vector<Application::ActionDefinition> actionDefinitions;

    QOpenGLWidget &shareWidget;
    GLuint brushVertexBuffer;
    QHash<QString, QOpenGLShader *> shaders;
    QHash<QString, QOpenGLShaderProgram *> programs;
    Session session;
    QSettings settings;
    QHash<QString, QAction *> actions;
    QHash<QString, QImage *> iconSheets;
    QHash<QString, QIcon *> iconCache;

    explicit Application(int &argc, char **argv);
    ~Application();
    bool addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src);
    bool addProgram(const QString &name, const QStringList &shaderList);
    GLuint shader(const QString &name) { return shaders[name]->shaderId(); }
    GLuint program(const QString &name) { return programs[name]->programId(); }
    QIcon icon(const QString &sheet, const QString &name, const int scale = 1);

private:
    GLuint processShader(const GLenum type, const QString &src);
    GLuint processProgram(const QStringList &shaders);
    void createActions();
};

#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
