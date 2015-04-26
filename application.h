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
    Q_OBJECT
public:
    static const QString fileDialogFilterString;

    QOffscreenSurface offScreen;
    QOpenGLContext context;
    QOpenGLWidget &shareWidget;
    GLuint brushVertexBuffer;
    QHash<QString, QOpenGLShader *> shaders;
    QHash<QString, QOpenGLShaderProgram *> programs;
    Session session;
    QSettings settings;
    QHash<QString, QAction *> actions;
    QHash<QString, QMenu *> menus;
    QHash<QString, QImage> iconSheets;
    QHash<QString, QIcon> iconCache;

    explicit Application(int &argc, char **argv);
    virtual ~Application();
    bool addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src);
    bool addProgram(const QString &name, const QStringList &shaderList);
    GLuint shader(const QString &name) { return shaders[name]->shaderId(); }
    GLuint program(const QString &name) { return programs[name]->programId(); }
    QIcon icon(const QString &sheet, const QString &name, const int scale = 1);

    Document *activeDocument();
    Editor *activeEditor();
    QMenuBar *menuBar();

signals:
    void activeDocumentChanged(Document *document);
    void activeEditorChanged(Editor *editor);

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
    bool editorNew();
    bool editorClone();
    bool editorClose();
    void about();
    void aboutQt();
    void license();

private:
    static const GLfloat brushVertices[][2];
    struct ActionDefinition {
        QString name;
        QString text;
        QString icon;
        bool checkable;
        bool checked;
        int standardShortcut;
        QString customShortcut;
        QString iconText;
        QString toolTip;
        QString statusTip;
        QString menuName;
    };
    static const QList<Application::ActionDefinition> actionDefinitions;
    struct MenuDefinition {
        QString name;
        QString text;
        QList<QString> actionNames;
    };
    static const QList<Application::MenuDefinition> menuDefinitions;
    static const int iconSheetWidth;

    void createActions();
    void createMenus();
    void setActionMenus();
    static QMenuBar *menuBarFromMenu(QMenu *menu);
    Document *m_activeDocument;
};

#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
