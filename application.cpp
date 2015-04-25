#include "application.h"
#include "mainwindow.h"
#include "util.h"
#include <QDirIterator>
#include <exception>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

const GLfloat Application::brushVertices[][2] = {
    {-1.f, -1.f},
    {1.f, -1.f},
    {1.f, 1.f},
    {-1.f, 1.f},
};

const QString Application::fileDialogFilterString = tr(
            "All Image Files (*.png *.gif *.bmp *.jpeg *.jpg);;"
            "PNG Image Files (*.png);;"
            "GIF Image Files (*.gif);;"
            "BMP Image Files (*.bmp);;"
            "JPEG Image Files (*.jpeg *.jpg);;");

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), shaders(), programs(), session(), actions(),
    shareWidget((({
        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        QSurfaceFormat::setDefaultFormat(format);
        }), *new QOpenGLWidget())),
    offScreen(), context()
{
    setWindowIcon(QIcon(":/images/simpix-48x48.png"));

    setApplicationName("SimPix");
    setOrganizationName("Uninhabitant");
    setOrganizationDomain("uninhabitant.com");
    setApplicationVersion("0.0.1");

    setStyleSheet(
//        "QStatusBar::item {border: none}"
//        "* {color: #fff; background-color: #000}"
//        "* {font-size: 24pt}"
//        "* {icon-size: 48px}"
//        "* {margin: 0px; border: 0px; padding: 0px}"
//        "* {margin: 1px; border: 1px; padding: 1px}"
//        "QSpinBox, QDoubleSpinBox {margin: 1px; border: 1px; padding: 1px}"
        ""
        );

    swatchBackgroundPixmap = generateBackgroundPixmap(16);

    iconSheets.insert("cursors", new QImage(":/images/cursors.png"));
    iconSheets.insert("actions", new QImage(":/images/actions.png"));
    iconSheets.insert("objects", new QImage(":/images/objects.png"));

    createActions();
    createMenus();
    setActionMenus();

    auto formatInfo = [](const QSurfaceFormat &format, const QString &label) {
        qDebug() <<  qPrintable(label) << "Format:" << "Major" << format.majorVersion() << "Minor" << format.minorVersion() << "Profile" << format.profile();
    };

//    offScreen.setFormat(QSurfaceFormat::defaultFormat());
//    offScreen.create();
//    context.setFormat(QSurfaceFormat::defaultFormat());
//    context.create();
//    context.makeCurrent(&offScreen);
//    formatInfo(offScreen.format(), "Off Screen");
//    formatInfo(context.format(), "Context");

    formatInfo(QSurfaceFormat::defaultFormat(), "Default");

    // Show to initialize share widget
    shareWidget.show();
    shareWidget.hide();
    qDebug() << "Context valid:" << shareWidget.context()->isValid();
    formatInfo(shareWidget.context()->format(), "Share");
    shareWidget.makeCurrent();

//    shareWidget.context()->setShareContext(&context);
//    shareWidget.context()->create();
//    shareWidget.makeCurrent();
//    qDebug() << "Sharing?" << QOpenGLContext::areSharing(&context, shareWidget.context());
//    qDebug() << "Context valid:" << shareWidget.context()->isValid();
//    QOpenGLWidget *widget = new QOpenGLWidget();
//    widget->show();
//    widget->hide();
//    widget->context()->setShareContext(&context);
//    widget->context()->create();
//    widget->makeCurrent();
//    qDebug() << "Sharing?" << QOpenGLContext::areSharing(&context, widget->context());
//    qDebug() << "Context valid:" << widget->context()->isValid();
//    qDebug() << "Sharing?" << QOpenGLContext::areSharing(shareWidget.context(), widget->context());

    {
        GLContextGrabber grab(&shareWidget);
//        GLContextGrabber grab(&context, &offScreen);
        initializeOpenGLFunctions();

        glGenBuffers((GLsizei)1, &brushVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, brushVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), brushVertices, GL_STATIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);

        static const QHash<QString, QOpenGLShader::ShaderTypeBit> extensionToShaderType = {
            { "vert", QOpenGLShader::Vertex },
            { "frag", QOpenGLShader::Fragment },
            { "geom", QOpenGLShader::Geometry },
        };
        QDirIterator iterator(":/shaders");
        while (iterator.hasNext()) {
            iterator.next();
            QFileInfo info = iterator.fileInfo();
            addShader(info.fileName(), extensionToShaderType[info.completeSuffix()], fileToString(info.filePath()));
        }
        addProgram("image", {"canvastiled.vert", "image.frag"});
        addProgram("frame", {"canvassingle.vert", "frame.frag"});
        addProgram("checkerboard", {"viewport.vert", "checkerboard.frag"});
        addProgram("brushellipse", {"brush.vert", "brushellipse.frag"});
        addProgram("brushrectangle", {"brush.vert", "brushrectangle.frag"});
    }

    MainWindow *window = new MainWindow;
    session.windows.append(window);
    window->show();
}

Application::~Application()
{
    {
        GLContextGrabber grab(&shareWidget);

        qDeleteAll(programs);
        qDeleteAll(shaders);
        glDeleteBuffers((GLsizei)1, &brushVertexBuffer);
    }

    QListIterator<QWidget *> window(session.windows);
    while (window.hasNext()) {
        window.next()->deleteLater();
    }
    shareWidget.deleteLater();

    qDeleteAll(actions);
    qDeleteAll(iconSheets);
    qDeleteAll(iconCache);

    delete swatchBackgroundPixmap;
}

bool Application::addShader(const QString &name, const QOpenGLShader::ShaderType type, const QString &src)
{
    bool error = false;
    QString errorText;
    QOpenGLShader *shader;
    error |= !(shader = new QOpenGLShader(type));
    if (error) {
        errorText += shader->log();
    }
    error |= !shader->compileSourceCode(src);
    if (error) {
        errorText += shader->log();
    }
    if (!error) {
        shaders.insert(name, shader);
    }
    else {
        delete shader;
        qDebug() << errorText;
    }
    return error;
}

bool Application::addProgram(const QString &name, const QStringList &shaderList)
{
    bool error = false;
    QString errorText;
    QOpenGLShaderProgram *program;
    error |= !(program = new QOpenGLShaderProgram);
    if (error) {
        errorText += program->log();
    }
    QStringListIterator shader(shaderList);
    while (!error && shader.hasNext()) {
        QOpenGLShader *value;
        if (error |= !(value = shaders.value(shader.next()))) {
            errorText += program->log();
            break;
        }
        if (error |= !program->addShader(value)) {
            errorText += program->log();
            break;
        }
    }
    !error && (error |= !program->link());
    if (error) {
        errorText += program->log();
    }
    if (!error) {
        programs.insert(name, program);
    }
    else {
        delete program;
        qDebug() << errorText;
    }
    return error;
}

const int Application::iconSheetWidth = 16;

QIcon Application::icon(const QString &sheet, const QString &name, const int scale)
{
    const QImage &image = *iconSheets[sheet];
    const int size = int(floor(image.width() / iconSheetWidth));
    return QIcon();
}

Document *Application::activeDocument()
{
    return nullptr;
}

Editor *Application::activeEditor()
{
    MainWindow *window = dynamic_cast<MainWindow *>(activeWindow());
    if (window) {

    }
    return nullptr;
}

void Application::documentNew()
{

}

void Application::documentOpen()
{

}

bool Application::documentSave()
{

}

bool Application::documentSaveAs()
{

}

bool Application::documentClose()
{

}

bool Application::editorNew()
{

}

bool Application::editorClose()
{

}

void Application::about()
{
    QFile data(":/text/ABOUT");
    QString text;
    if (data.open(QFile::ReadOnly)) {
        text = QTextStream(&data).readAll();
    }
    QMessageBox::about(activeWindow(), tr(QString("About %1").arg(applicationName()).toLatin1()),
         text.arg(applicationName()));
}

void Application::aboutQt()
{
    QMessageBox::aboutQt(activeWindow());
}

void Application::license()
{
    QFile data(":/text/LICENSE");
    QString text;
    if (data.open(QFile::ReadOnly)) {
        text = QTextStream(&data).readAll();
    }
    QMessageBox::information(activeWindow(), QString(), text);
}

const QList<Application::ActionDefinition> Application::actionDefinitions = {
    {"applicationMenu", "&Application", nullptr, -1, nullptr, nullptr, nullptr, nullptr, "application"},
    {"applicationAbout", "&About", "help-about", -1, nullptr, nullptr, "About application.", nullptr, nullptr},
    {"applicationAboutQt", "About &Qt", nullptr, -1, nullptr, nullptr, "About Qt.", nullptr, nullptr},
    {"applicationExit", "&Exit", "application-exit", QKeySequence::Quit, nullptr, nullptr, "Exit application.", nullptr, nullptr},

    {"sessionMenu", "&Session", nullptr, -1, nullptr, nullptr, nullptr, nullptr, "session"},
    {"sessionNew", "&New", nullptr, -1, nullptr, "New Session", "Open new session.", nullptr, nullptr},
    {"sessionOpen", "&Open", nullptr, -1, nullptr, "Open Session", "Open existing session.", nullptr, nullptr},
    {"sessionSave", "&Save", nullptr, -1, nullptr, "Save Session", "Save current session.", nullptr, nullptr},
    {"sessionSaveAs", "Save &As", nullptr, -1, nullptr, "Save Session As" ,"Save current session as new filename.", nullptr, nullptr},
    {"sessionClose", "&Close", nullptr, -1, nullptr, "Close Session", "Close current session.", nullptr, nullptr},

    {"windowFullScreen", "&Full Screen", "view-fullscreen", QKeySequence::FullScreen, nullptr, nullptr, "Make window full screen.", nullptr, nullptr},

    {"documentMenu", "&Document", nullptr, -1, nullptr, nullptr, nullptr, nullptr, "document"},
    {"documentNew", "&New", "document-new", QKeySequence::New, nullptr, "New Document", "Open new document.", nullptr, nullptr},
    {"documentOpen", "&Open", "document-open", QKeySequence::Open, nullptr, "Open Document", "Open existing document.", nullptr, nullptr},
    {"documentSave", "&Save", "document-save", QKeySequence::Save, nullptr, "Save Document", "Save current document.", nullptr, nullptr},
    {"documentSaveAs", "Save &As", "document-save-as", QKeySequence::SaveAs, nullptr, "Save Document As" ,"Save current document as new filename.", nullptr, nullptr},
    {"documentClose", "&Close", "document-close", -1, nullptr, "Close Document", "Close current document.", nullptr, nullptr},

    {"editorMenu", "&Editor", nullptr, -1, nullptr, nullptr, nullptr, nullptr, "editor"},
    {"editorNew", "&New", "document-new", -1, nullptr, "New Editor", "Open new editor.", nullptr, nullptr},
    {"editorClose", "&Close", "document-close", QKeySequence::Close, nullptr, "Close Editor", "Close current editor.", nullptr, nullptr},
};

void Application::createActions()
{
    for (int i = 0; i < Application::actionDefinitions.size(); i++) {
        const Application::ActionDefinition &definition = Application::actionDefinitions[i];
        QAction *action = new QAction(QIcon::fromTheme(definition.icon), definition.text, nullptr);
        if (definition.standardShortcut >= 0) {
            action->setShortcut(static_cast<enum QKeySequence::StandardKey>(definition.standardShortcut));
            action->setShortcutContext(Qt::ApplicationShortcut);
        }
        else if (!definition.customShortcut.isEmpty()) {
            action->setShortcut(QKeySequence(definition.customShortcut));
            action->setShortcutContext(Qt::ApplicationShortcut);
        }
        if (!definition.iconText.isEmpty()) {
            action->setIconText(definition.iconText);
        }
        if (!definition.toolTip.isEmpty()) {
            action->setToolTip(definition.toolTip);
        }
        if (!definition.statusTip.isEmpty()) {
            action->setStatusTip(definition.statusTip);
        }
        actions[definition.name] = action;
    }
//    qDebug() << actions;
}

const QList<Application::MenuDefinition> Application::menuDefinitions = {
    {"main", "&Menu", {"applicationMenu", "sessionMenu", "documentMenu"}},
    {"application", "&Application", {"applicationAbout", "applicationAboutQt", nullptr, "applicationExit"}},
    {"session", "&Session", {"sessionNew", "sessionOpen", nullptr, "sessionSave", "sessionSaveAs", nullptr, "sessionClose"}},
    {"document", "&Document", {"documentNew", "documentOpen", nullptr, "documentSave", "documentSaveAs", nullptr, "documentClose"}},
    {"editor", "&Editor", {"editorNew", nullptr, "editorClose"}},
};

void Application::createMenus()
{
    for (int i = 0; i < Application::menuDefinitions.size(); i++) {
        const Application::MenuDefinition &definition = Application::menuDefinitions[i];
        QMenu *menu = new QMenu(definition.text);
        for (int j = 0; j < definition.actionNames.size(); j++) {
            if (!definition.actionNames[j].isEmpty()) {
                menu->addAction(actions[definition.actionNames[j]]);
            }
            else {
                menu->addSeparator();
            }
        }
        menus[definition.name] = menu;
    }
    qDebug() << menus;
}

void Application::setActionMenus()
{
    for (int i = 0; i < Application::actionDefinitions.size(); i++) {
        const Application::ActionDefinition &definition = Application::actionDefinitions[i];
        if (!definition.menuName.isEmpty()) {
            actions[definition.name]->setMenu(menus[definition.menuName]);
            qDebug() << definition.menuName;
        }
    }
}
