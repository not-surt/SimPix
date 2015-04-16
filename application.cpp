#include "application.h"
#include "mainwindow.h"
#include <QDebug>
#include "util.h"
#include <QDirIterator>
#include <exception>

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
            "JPEG Image Files (*.jpeg *.jpg)");

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), m_shaders(), m_programs(), m_window(nullptr), m_shareWidget(nullptr), m_session(new Session)
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

    QSurfaceFormat format;
    format.setVersion(3, 3);
//    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);
    qDebug() << "Default" << "Version:" << "Major" << QSurfaceFormat::defaultFormat().majorVersion() << "Minor" << QSurfaceFormat::defaultFormat().minorVersion() << "Profile" << QSurfaceFormat::defaultFormat().profile();

    m_window = new MainWindow;

    m_shareWidget = new QOpenGLWidget;
//    m_shareWidget->setFormat(format); // For windows?
    // Show to initialize share widget
    m_shareWidget->show();
    m_shareWidget->hide();
    qDebug() << "Context valid:" << m_shareWidget->context()->isValid();
    qDebug() << "Share" << "Version:" << "Major" << m_shareWidget->context()->format().majorVersion() << "Minor" << m_shareWidget->context()->format().minorVersion() << "Profile" << m_shareWidget->context()->format().profile();
    m_shareWidget->makeCurrent();

    {
        GLContextGrabber grab(m_shareWidget);
        initializeOpenGLFunctions();

        glGenBuffers((GLsizei)1, &brushVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, brushVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), brushVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    m_window->show();
}

Application::~Application()
{
    {
        GLContextGrabber grab(m_shareWidget);

        QHashIterator<QString, QOpenGLShaderProgram *> program(m_programs);
        while (program.hasNext()) {
            delete *program.next();
        }

        QHashIterator<QString, QOpenGLShader *> shader(m_shaders);
        while (shader.hasNext()) {
            delete *shader.next();
        }

        glDeleteBuffers((GLsizei)1, &brushVertexBuffer);
    }

    m_window->deleteLater();
    m_shareWidget->deleteLater();

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
        m_shaders.insert(name, shader);
    }
    else {
        delete shader;
        qDebug() << errorText;
    }
    return error;
}

bool Application::addProgram(const QString &name, const QStringList &shaders)
{
    bool error = false;
    QString errorText;
    QOpenGLShaderProgram *program;
    error |= !(program = new QOpenGLShaderProgram);
    if (error) {
        errorText += program->log();
    }
    QStringListIterator shader(shaders);
    while (!error && shader.hasNext()) {
        QOpenGLShader *value;
        if (error |= !(value = m_shaders.value(shader.next()))) {
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
        m_programs.insert(name, program);
    }
    else {
        delete program;
        qDebug() << errorText;
    }
    return error;
}
