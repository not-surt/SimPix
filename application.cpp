#include "application.h"
#include "mainwindow.h"
#include <QDebug>
#include <QOpenGLShader>
#include "canvaswindow.h"
#include "util.h"
#include <exception>

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), m_format(), m_context(), m_offscreen(), m_samplers(), m_shaders(), m_programs(), m_window()
{
    setWindowIcon(QIcon(":/images/simpix-48x48.png"));

    setApplicationName("SimPix");
    setOrganizationName("Uninhabitant");
    setOrganizationDomain("uninhabitant.com");
    setApplicationVersion("0.0.1");

    setStyleSheet(
        "QStatusBar::item {border: none}"
//        "* {color: #fff; background-color: #000}"
//        "* {font-size: 24pt}"
//        "* {icon-size: 48px}"
//        "* {margin: 0px; border: 0px; padding: 0px}"
        ""
        );

    m_format.setRenderableType(QSurfaceFormat::OpenGL);
    m_format.setMajorVersion(2);
    m_format.setMinorVersion(1);
    m_format.setProfile(QSurfaceFormat::NoProfile);
    m_format.setSamples(8);

    m_offscreen.setFormat(m_format);
    m_offscreen.create();
    if (m_offscreen.format() != m_offscreen.requestedFormat()) {
        qDebug() << "Couldn't set requested OpenGL surface format!" << endl << "Requested: " << m_offscreen.requestedFormat() << endl << "Created: " << m_offscreen.format();
    }

//    m_context.setFormat(m_format);
    m_context.setFormat(m_offscreen.format());
    m_context.create();

    contextMakeCurrent();
    initializeOpenGLFunctions();

    addShader("image.vert", QOpenGLShader::Vertex, fileToString(":/shaders/image.vert"));
    addShader("image.frag", QOpenGLShader::Fragment, fileToString(":/shaders/image.frag"));
    addProgram("image", QStringList()
               << "image.vert"
               << "image.frag");
    contextDoneCurrent();

    m_window.show();
}

Application::~Application()
{
    QHashIterator<QString, QOpenGLShader *> shader(m_shaders);
    while (shader.hasNext()) {
        delete *shader.next();
    }

    QHashIterator<QString, QOpenGLShaderProgram *> program(m_programs);
    while (program.hasNext()) {
        delete *program.next();
    }
}

void Application::contextMakeCurrent()
{
    m_context.makeCurrent(&m_offscreen);
}

void Application::contextDoneCurrent()
{
    m_context.doneCurrent();
}

QSurfaceFormat &Application::format()
{
    return m_format;
}

QOpenGLContext &Application::context()
{
    return m_context;
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

GLuint Application::shader(const QString &name)
{
    return m_shaders.value(name)->shaderId();
}

bool Application::addProgram(const QString &name, const QStringList &shaders)
{
    bool error = false;
    QString errorText;
    QOpenGLShaderProgram *program;
    error |= !(program = new QOpenGLShaderProgram(&m_context));
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
    return m_programs.value(name)->programId();
}
