#include "application.h"
#include "mainwindow.h"
#include <QDebug>
#include "util.h"
#include <exception>

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

//    QSurfaceFormat format;
//    format.setRenderableType(QSurfaceFormat::OpenGL);
//    format.setVersion(3, 3);
//    format.setProfile(QSurfaceFormat::CoreProfile);
//    QSurfaceFormat::setDefaultFormat(format);

    m_window = new MainWindow;

    m_shareWidget = new QOpenGLWidget;
    // Hack to initialize share widget
    m_shareWidget->show();
    m_shareWidget->hide();
    qDebug() << "Share:" << m_shareWidget->context() << m_shareWidget->context()->isValid() << "Sharing:" << m_shareWidget->context()->shareContext();

    {
        ContextGrabber grab(m_shareWidget);
        addShader("canvas.vert", QOpenGLShader::Vertex, fileToString(":/shaders/canvas.vert"));

        addShader("image.frag", QOpenGLShader::Fragment, fileToString(":/shaders/image.frag"));
        addProgram("image", QStringList() << "canvas.vert" << "image.frag");

        addShader("frame.frag", QOpenGLShader::Fragment, fileToString(":/shaders/frame.frag"));
        addProgram("frame", QStringList() << "canvas.vert" << "frame.frag");

        addShader("checkerboard.frag", QOpenGLShader::Fragment, fileToString(":/shaders/checkerboard.frag"));
        addProgram("checkerboard", QStringList() << "canvas.vert" << "checkerboard.frag");

        addShader("brush.vert", QOpenGLShader::Vertex, fileToString(":/shaders/brush.vert"));

        addShader("ellipsebrush.frag", QOpenGLShader::Fragment, fileToString(":/shaders/ellipsebrush.frag"));
        addProgram("ellipsebrush", QStringList() << "brush.vert" << "ellipsebrush.frag");

        addShader("rectanglebrush.frag", QOpenGLShader::Fragment, fileToString(":/shaders/rectanglebrush.frag"));
        addProgram("rectanglebrush", QStringList() << "brush.vert" << "rectanglebrush.frag");
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

QOpenGLWidget *Application::shareWidget()
{
    return m_shareWidget;
}

//void Application::openImage(const QString &path)
//{
//    m_images[path] = new Image(path);
//}

//bool Application::removeImage(const QString &path)
//{
//    return (m_images.remove(path) > 0);
//}

//Image *Application::image(const QString &path)
//{
//    return m_images[path];
//}

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

GLuint Application::shader(const QString &name)
{
    return m_shaders[name]->shaderId();
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

GLuint Application::program(const QString &name)
{
    return m_programs[name]->programId();
}
