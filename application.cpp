#include "application.h"
#include "mainwindow.h"
#include <QDebug>
#include "util.h"
#include <QDirIterator>
#include <exception>

const QString Application::fileDialogFilterString = tr(
            "All Image Files (*.png *.gif *.bmp *.jpeg *.jpg);;"
            "PNG Image Files (*.png);;"
            "GIF Image Files (*.gif);;"
            "BMP Image Files (*.bmp);;"
            "JPEG Image Files (*.jpeg *.jpg)");

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), m_shaders(), m_programs(), m_window(nullptr), m_shareWidget(nullptr)
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

    // Why cause fail?
    QSurfaceFormat format;
//    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
//    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    m_window = new MainWindow;

    m_shareWidget = new QOpenGLWidget;
    // Hack to initialize share widget
    m_shareWidget->show();
    qDebug() << "Context valid:" << m_shareWidget->context()->isValid();
    qDebug() << "Version:" << "Major" << m_shareWidget->context()->format().majorVersion() << "Minor" << m_shareWidget->context()->format().minorVersion();
    m_shareWidget->hide();

    {
        ContextGrabber grab(m_shareWidget);
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
        ContextGrabber grab(m_shareWidget);
        QHashIterator<QString, QOpenGLShader *> shader(m_shaders);
        while (shader.hasNext()) {
            delete *shader.next();
        }

        QHashIterator<QString, QOpenGLShaderProgram *> program(m_programs);
        while (program.hasNext()) {
            delete *program.next();
        }
    }

    delete m_window;
    delete m_shareWidget;
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
