#include "application.h"
#include "mainwindow.h"
#include "util.h"
#include <QDirIterator>
#include <exception>
#include <QAction>

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
//        format.setProfile(QSurfaceFormat::CompatibilityProfile);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        QSurfaceFormat::setDefaultFormat(format);
    }), *new QOpenGLWidget()))
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

    auto formatInfo = [](const QSurfaceFormat &format, const QString &label) {
        qDebug() <<  qPrintable(label) << "Format:" << "Major" << format.majorVersion() << "Minor" << format.minorVersion() << "Profile" << format.profile();
    };

    formatInfo(QSurfaceFormat::defaultFormat(), "Default");

//    m_shareWidget = new QOpenGLWidget;
//    shareWidget.setFormat(format); // For windows?
    // Show to initialize share widget
    shareWidget.show();
    shareWidget.hide();
    qDebug() << "Context valid:" << shareWidget.context()->isValid();
    formatInfo(shareWidget.context()->format(), "Share");
    shareWidget.makeCurrent();

    {
        GLContextGrabber grab(&shareWidget);
        initializeOpenGLFunctions();

        glGenBuffers((GLsizei)1, &brushVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, brushVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), brushVertices, GL_STATIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);

        QDirIterator iterator(":/shaders");
        while (iterator.hasNext()) {
            iterator.next();
            QFileInfo info = iterator.fileInfo();
            addShader(info.fileName(), info.completeSuffix() == "vert" ? QOpenGLShader::Vertex : QOpenGLShader::Fragment, fileToString(info.filePath()));
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

//        QHashIterator<QString, QOpenGLShaderProgram *> program(programs);
//        while (program.hasNext()) {
//            delete *program.next();
//        }
        qDeleteAll(programs);

//        QHashIterator<QString, QOpenGLShader *> shader(shaders);
//        while (shader.hasNext()) {
//            delete *shader.next();
//        }
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

QIcon Application::icon(const QString &sheet, const QString &name, const int scale)
{
    const QImage &image = *iconSheets[sheet];
    const int size = int(floor(image.width() / 16));
    return QIcon();
}

void Application::createActions()
{
    actions["documentNew"] = new QAction(QIcon::fromTheme("document-new"), QString("New"), nullptr);
    actions["documentOpen"] = new QAction(QIcon::fromTheme("document-open"), QString("Open"), nullptr);
    actions["documentSave"] = new QAction(QIcon::fromTheme("document-save"), QString("Save"), nullptr);
    actions["documentSaveAs"] = new QAction(QIcon::fromTheme("document-save-as"), QString("Save As"), nullptr);
    actions["documentClose"] = new QAction(QIcon::fromTheme("document-close"), QString("Close"), nullptr);
}
