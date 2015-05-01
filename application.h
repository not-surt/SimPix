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

struct ActionOwner {
    struct ActionDefinition {
        QString text;
        QString icon;
        bool checkable;
        bool checked;
        int standardShortcut;
        QString customShortcut;
        QString toolTip;
        QString menuName;
        QString groupName;

        QAction *toAction() const {
            QAction *action = new QAction(QIcon::fromTheme(icon), text, nullptr);
            action->setCheckable(checkable);
            action->setChecked(checked);
            if (standardShortcut >= 0) {
                action->setShortcut(static_cast<enum QKeySequence::StandardKey>(standardShortcut));
                action->setShortcutContext(Qt::ApplicationShortcut);
            }
            else if (!customShortcut.isEmpty()) {
                action->setShortcut(QKeySequence(customShortcut));
                action->setShortcutContext(Qt::ApplicationShortcut);
            }
            if (!toolTip.isEmpty()) {
                action->setToolTip(toolTip);
            }

            return action;
        }
    };
    struct MenuDefinition {
        QString text;
        QList<QString> actionNames;

        QMenu *toMenu(QHash<QString, QAction *> &actions) const {
            QMenu *menu = new QMenu(text);
            for (int j = 0; j < actionNames.size(); j++) {
                if (!actionNames[j].isEmpty()) {
                    menu->addAction(actions[actionNames[j]]);
                }
                else {
                    menu->addSeparator();
                }
            }

            return menu;
        }
    };

    QHash<QString, QAction *> actions;
    QHash<QString, QActionGroup *> actionGroups;
    QHash<QString, QMenu *> menus;

    explicit ActionOwner(const QHash<QString, ActionDefinition> actionDefinitions, const QHash<QString, MenuDefinition> menuDefinitions) {
        // Create actions
        for (const QString key : actionDefinitions.keys()) {
            actions[key] = actionDefinitions[key].toAction();
        }
        // Set action groups
        for (const QString key : actionDefinitions.keys()) {
            const ActionDefinition &definition = actionDefinitions[key];
            if (!definition.groupName.isEmpty()) {
                if (!actionGroups[definition.groupName]) {
                    actionGroups[definition.groupName] = new QActionGroup(nullptr);
                }
                actions[key]->setActionGroup(actionGroups[definition.groupName]);
            }
        }
        // Create menus
        for (const QString key : menuDefinitions.keys()) {
            menus[key] = menuDefinitions[key].toMenu(actions);
        }
        // Set menu actions
        for (const QString key : actionDefinitions.keys()) {
            const ActionDefinition &definition = actionDefinitions[key];
            if (!definition.menuName.isEmpty()) {
                actions[key]->setMenu(menus[definition.menuName]);
                qDebug() << definition.menuName;
            }
        }
    }
};

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

//    void activateWindow(MainWindow *window);
//    void activateDocument(Document *document);
//    void activateEditor(Editor *editor);

//    MainWindow *activeWindow();
//    Document *activeDocument();
//    Editor *activeEditor();

    QMenuBar *createMenuBar();

signals:
//    void activeDocumentChanged(Document *document);
//    void activeEditorChanged(Editor *editor);

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
    static const QHash<QString, Application::ActionDefinition> actionDefinitions;
    static const QHash<QString, Application::MenuDefinition> menuDefinitions;
    static const int iconSheetWidth;

    void initializeGL();
//    Document *m_activeDocument;
};

#define APP (static_cast<Application *>(qApp))

#endif // APPLICATION_H
