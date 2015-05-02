#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QOpenGLFunctions_3_3_Core>
#include <QSettings>
//#include <QMenu>
//#include <QAction>
#include <QMenuBar>
#include <QHash>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QKeySequence>
#include <QActionGroup>
#include "session.h"
#include "settingsdialog.h"
#include "actionowner.h"

class Window;
class Editor;
class Document;

class Application : public QApplication, public QOpenGLFunctions_3_3_Core, public ActionOwner
{
    Q_OBJECT
public:
    static const QString fileDialogFilterString;

    QOffscreenSurface offScreen;
    QOpenGLContext context;
    QOpenGLWidget &shareWidget;
    SettingsDialog settingsWindow;
    GLuint brushVertexBuffer;
    QHash<QString, QOpenGLShader *> shaders;
    QHash<QString, QOpenGLShaderProgram *> programs;
    Session session;
    QSettings settings;
    QHash<QString, QImage> iconSheets;
    QHash<QString, QIcon> iconCache;

    explicit Application(int &argc, char **argv);
    virtual ~Application();
    bool addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src);
    bool addProgram(const QString &name, const QStringList &shaderList);
    GLuint shader(const QString &name) { return shaders[name]->shaderId(); }
    GLuint program(const QString &name) { return programs[name]->programId(); }
    QIcon icon(const QString &sheet, const QString &name, const int scale = 1);

public slots:
    void sessionNew();
    void sessionOpen();
    bool sessionSave();
    bool sessionSaveAs();
    bool sessionClose();
    bool windowNew();
    bool windowClone();
    bool windowClose();
    void documentNew();
    void documentOpen();
    bool documentSave();
    bool documentSaveAs();
    bool documentClose();
    void about();
    void aboutQt();
    void license();

private:
    static const GLfloat brushVertices[][2];
    static const QHash<QString, ActionDefinition> actionDefinitions;
    static const QHash<QString, MenuDefinition> menuDefinitions;
    static const int iconSheetWidth;

    void initializeGL();
};

#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
