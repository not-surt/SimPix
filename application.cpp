#include "application.h"
#include "window.h"
#include "util.h"
#include <QDirIterator>
#include <exception>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include "newdialog.h"
#include "imagedocument.h"
#include <QFileDialog>
#include <QMenuBar>

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
    QApplication(argc, argv), ActionOwner(actionDefinitions, menuDefinitions), shaders(), programs(), session(),
    shareWidget((({
        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        QSurfaceFormat::setDefaultFormat(format);
        }), *new QOpenGLWidget())),
    offScreen(), context(), settingsWindow()
{
    setWindowIcon(QIcon(":/images/simpix-48x48.png"));

    setApplicationName("SimPix");
    setApplicationVersion("0.0.1");
    setOrganizationName("Uninhabitant");
    setOrganizationDomain("uninhabitant.com");

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

    iconSheets.insert("cursors", QImage(":/images/cursors.png"));
    iconSheets.insert("actions", QImage(":/images/actions.png"));
    iconSheets.insert("objects", QImage(":/images/objects.png"));

    initializeGL();

    QObject::connect(actions["applicationAbout"], &QAction::triggered, this, &Application::about);
    QObject::connect(actions["applicationAboutQt"], &QAction::triggered, this, &Application::aboutQt);
    QObject::connect(actions["applicationLicense"], &QAction::triggered, this, &Application::license);
    QObject::connect(actions["applicationSettings"], &QAction::triggered, &settingsWindow, &SettingsDialog::show);
    QObject::connect(actions["applicationExit"], &QAction::triggered, this, &Application::closeAllWindows);

    QObject::connect(actions["windowNew"], &QAction::triggered, this, &Application::windowNew);
    QObject::connect(actions["windowClone"], &QAction::triggered, this, &Application::windowClone);
    QObject::connect(actions["windowClose"], &QAction::triggered, this, &Application::windowClose);

    QObject::connect(actions["documentNew"], &QAction::triggered, this, &Application::documentNew);
    QObject::connect(actions["documentOpen"], &QAction::triggered, this, &Application::documentOpen);
    QObject::connect(actions["documentSave"], &QAction::triggered, this, &Application::documentSave);
    QObject::connect(actions["documentSaveAs"], &QAction::triggered, this, &Application::documentSaveAs);
    QObject::connect(actions["documentClose"], &QAction::triggered, this, &Application::documentClose);

    QObject::connect(actions["sessionNew"], &QAction::triggered, this, &Application::sessionNew);
    QObject::connect(actions["sessionOpen"], &QAction::triggered, this, &Application::sessionOpen);
    QObject::connect(actions["sessionSave"], &QAction::triggered, this, &Application::sessionSave);
    QObject::connect(actions["sessionSaveAs"], &QAction::triggered, this, &Application::sessionSaveAs);
    QObject::connect(actions["sessionClose"], &QAction::triggered, this, &Application::sessionClose);

    windowNew();
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
    const QImage &image = iconSheets[sheet];
    const int size = int(floor(image.width() / iconSheetWidth));
    return QIcon();
}

void Application::sessionNew()
{

}

void Application::sessionOpen()
{

}

bool Application::sessionSave()
{

}

bool Application::sessionSaveAs()
{

}

bool Application::sessionClose()
{

}

bool Application::windowNew()
{
    Window *window = new Window;
    session.windows.append(window);
    window->show();
}

bool Application::windowClone()
{

}

bool Application::windowClose()
{
    Window *window = dynamic_cast<Window *>(activeWindow());
    window->close();
}

void Application::documentNew()
{
    Window *window = dynamic_cast<Window *>(activeWindow());
    if (window) {
        NewDialog *dialog = new NewDialog(window);
        if (dialog->exec()) {
            ImageDocument *image = new ImageDocument(APP->session, dialog->imageSize(), dialog->format());
    //        APP->session.documents.append(image);
            ImageEditor *editor = static_cast<ImageEditor *>(image->createEditor());
//            static_cast<SessionWidget *>(ui->dockWidgetSession->widget())->setSession(&APP->session);
            window->newEditorSubWindow(editor);
        }
        dialog->deleteLater();
    }
}

void Application::documentOpen()
{
    Window *window = dynamic_cast<Window *>(activeWindow());
    if (window) {
        APP->settings.beginGroup("file");
        QFileDialog dialog(window, tr("Open Image"), APP->settings.value("lastOpened", QDir::homePath()).toString(), APP->fileDialogFilterString);
        APP->settings.endGroup();
        APP->settings.beginGroup("window/open");
        dialog.restoreGeometry(APP->settings.value("geometry").toByteArray());
        dialog.exec();
        APP->settings.setValue("geometry", dialog.saveGeometry());
        QStringList fileNames = dialog.selectedFiles();
        APP->settings.endGroup();
        APP->settings.beginGroup("file");
//        QStringList fileNames = QFileDialog::getOpenFileNames(window, tr("Open Image"), APP->settings.value("lastOpened", QDir::homePath()).toString(), APP->fileDialogFilterString);
        QStringListIterator fileNameIterator(fileNames);
        QStringList failed;
        while (fileNameIterator.hasNext()) {
            QString fileName = fileNameIterator.next();
            APP->settings.setValue("lastOpened", fileName);
            ImageDocument *image = new ImageDocument(APP->session, fileName);
            if (!image->imageData()) {
                delete image;
                failed.append(QFileInfo(fileName).fileName());
            }
            else {
    //            APP->session.documents.append(image);
                ImageEditor *editor = static_cast<ImageEditor *>(image->createEditor());
//                static_cast<SessionWidget *>(ui->dockWidgetSession->widget())->setSession(&APP->session);
                window->newEditorSubWindow(editor);
            }
        }
        if (failed.length() > 0) {
            QMessageBox::critical(window, QString(), QString(tr("Error opening file(s) <b>\"%1\"</b>")).arg(failed.join(tr(", "))));
        }
        APP->settings.endGroup();
    }
}

bool Application::documentSave()
{
    Window *window = dynamic_cast<Window *>(activeWindow());
    if (window) {
        SubWindow *subWindow = static_cast<SubWindow *>(window->mdi->activeSubWindow());
        if (subWindow) {
            ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
            ImageDocument &image = static_cast<ImageDocument &>(editor->document);
            APP->settings.beginGroup("file");
            if (image.fileInfo.fileName().isNull()) {
                return documentSaveAs();
            }
            if (image.save()) {
                APP->settings.setValue("lastSaved", image.fileInfo.fileName());
                return true;
            }
            else {
                QMessageBox::critical(window, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(image.fileInfo.fileName()).fileName()));
            }
            APP->settings.endGroup();
        }
    }
    return false;
}

bool Application::documentSaveAs()
{
    Window *window = dynamic_cast<Window *>(activeWindow());
    if (window) {
        SubWindow *subWindow = static_cast<SubWindow *>(window->mdi->activeSubWindow());
        if (subWindow) {
            ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
            ImageDocument &image = static_cast<ImageDocument &>(editor->document);
            APP->settings.beginGroup("file");
            QString fileName;
            if (!image.fileInfo.fileName().isNull()) {
                fileName = image.fileInfo.fileName();
            }
            else {
                QFileInfo fileInfo(APP->settings.value("lastSaved", QDir::homePath()).toString());
                fileName = fileInfo.dir().path();
            }
            fileName = QFileDialog::getSaveFileName(window, tr("Save Image"), fileName, APP->fileDialogFilterString);
            if (!fileName.isNull()) {
                if (image.save(fileName)) {
                    APP->settings.setValue("lastSaved", fileName);
                }
                else {
                    QMessageBox::critical(window, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(fileName).fileName()));
                }
            }
            APP->settings.endGroup();
        }
    }
    return false;
}

bool Application::documentClose()
{
    Window *window = dynamic_cast<Window *>(activeWindow());
    if (window) {
        SubWindow *subWindow = static_cast<SubWindow *>(window->mdi->activeSubWindow());
        if (subWindow) {
            ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
            ImageDocument &image = static_cast<ImageDocument &>(editor->document);
            if (image.fileInfo.dirty()) {
                QString fileName = image.fileInfo.fileName().isNull() ? "<i>unnamed</>" : QFileInfo(image.fileInfo.fileName()).fileName();
                QMessageBox::StandardButton button = QMessageBox::question(window, QString(),
                    QString(tr("The file \"<b>%1</b>\" has unsaved changes.<br/>"
                               "Do you want to save it before closing?")).arg(fileName),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    QMessageBox::Save);
                if (button == QMessageBox::Cancel) {
                    return false;
                }
                if (button == QMessageBox::Save) {
                    if (!documentSave()) {
                        return false;
                    }
                }
                subWindow->close();
            }
            subWindow->close();
        }
    }
    return true;
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

void Application::initializeGL()
{
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

    GLContextGrabber grab(&shareWidget);
//    GLContextGrabber grab(&context, &offScreen);
    initializeOpenGLFunctions();

    glGenBuffers((GLsizei)1, &brushVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, brushVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), brushVertices, GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

const QHash<QString, Application::ActionDefinition> Application::actionDefinitions = {
    {"applicationMenu", {"&Application", nullptr, false, false, -1, nullptr, nullptr, "application", nullptr}},
    {"applicationAbout", {"&About", "help-about", false, false, -1, nullptr, "About application.", nullptr, nullptr}},
    {"applicationAboutQt", {"About &Qt", nullptr, false, false, -1, nullptr, "About Qt.", nullptr, nullptr}},
    {"applicationLicense", {"&License", nullptr, false, false, -1, nullptr, "Application license.", nullptr, nullptr}},
    {"applicationSettings", {"&Settings", nullptr, false, false, QKeySequence::Preferences, nullptr, "Application settings.", nullptr, nullptr}},
    {"applicationExit", {"&Exit", "application-exit", false, false, QKeySequence::Quit, nullptr, "Exit application.", nullptr, nullptr}},

    {"sessionMenu", {"&Session", nullptr, false, false, -1, nullptr, nullptr, "session", nullptr}},
    {"sessionNew", {"&New", "document-new", false, false, -1, nullptr, "Open new session.", nullptr, nullptr}},
    {"sessionOpen", {"&Open", "document-open", false, false, -1, nullptr, "Open existing session.", nullptr, nullptr}},
    {"sessionRecentMenu", {"&Recent", nullptr, false, false, -1, nullptr, nullptr, "sessionRecent", nullptr}},
    {"sessionSave", {"&Save", "document-save", false, false, -1, nullptr, "Save current session.", nullptr, nullptr}},
    {"sessionSaveAs", {"Save &As", "document-save-as", false, false, -1, nullptr, "Save current session as new filename.", nullptr, nullptr}},
    {"sessionClose", {"&Close", "document-close", false, false, -1, nullptr, "Close current session.", nullptr, nullptr}},

    {"layoutMenu", {"&Layout", nullptr, false, false, -1, nullptr, nullptr, "layout", nullptr}},
    {"layoutMenuBar", {"&Menu Bar", nullptr, true, true, -1, "Ctrl+M", "Toggle menu bar visibility.", nullptr, nullptr}},
    {"layoutStatusBar", {"&Status Bar", nullptr, true, true, -1, "Ctrl+B", "Toggle status bar visibility.", nullptr, nullptr}},

    {"documentMenu", {"&Document", nullptr, false, false, -1, nullptr, nullptr, "document", nullptr}},
    {"documentNew", {"&New", "document-new", false, false, QKeySequence::New, nullptr, "Open new document.", nullptr, nullptr}},
    {"documentOpen", {"&Open", "document-open", false, false, QKeySequence::Open, nullptr, "Open existing document.", nullptr, nullptr}},
    {"documentRecentMenu", {"&Recent", nullptr, false, false, -1, nullptr, nullptr, "documentRecent", nullptr}},
    {"documentSave", {"&Save", "document-save", false, false, QKeySequence::Save, nullptr, "Save current document.", nullptr, nullptr}},
    {"documentSaveAs", {"Save &As", "document-save-as", false, false, QKeySequence::SaveAs, nullptr, "Save current document as new filename.", nullptr, nullptr}},
    {"documentSaveAll", {"Save &All", nullptr, false, false, -1, nullptr, "Save All Documents", nullptr, nullptr}},
    {"documentClose", {"&Close", "document-close", false, false, -1, "Ctrl+Shift+W", "Close current document.", nullptr, nullptr}},
    {"documentCloseAll", {"Close &All", nullptr, false, false, -1, nullptr, "Close all documents.", nullptr, nullptr}},

    {"windowMenu", {"&Window", nullptr, false, false, -1, nullptr, nullptr, "window", nullptr}},
    {"windowNew", {"&New Window", "window-new", false, false, -1, nullptr, "Open new window.", nullptr, nullptr}},
    {"windowClone", {"&Clone Window", nullptr, false, false, -1, nullptr, "Clone current window.", nullptr, nullptr}},
    {"windowClose", {"&Close Window", "window-close", false, false, -1, nullptr, "Close current window.", nullptr, nullptr}},
    {"windowFullScreen", {"&Full Screen", "view-fullscreen", true, false, QKeySequence::FullScreen, nullptr, "Toggle window full screen.", nullptr, nullptr}},
    {"windowNext", {"&Next Window", nullptr, false, false, -1, nullptr, "Switch to next window.", nullptr, nullptr}},
    {"windowPrevious", {"&Previous Window", nullptr, false, false, -1, nullptr, "Switch to previous window.", nullptr, nullptr}},
};

const QHash<QString, Application::MenuDefinition> Application::menuDefinitions = {
    {"application", {"&Application", {"applicationAbout", "applicationAboutQt", "applicationLicense", nullptr, "applicationSettings", nullptr, "sessionMenu", "layoutMenu", nullptr, "applicationExit"}}},
    {"session", {"&Session", {"sessionNew", "sessionOpen", "sessionRecentMenu", nullptr, "sessionSave", "sessionSaveAs", nullptr, "sessionClose"}}},
    {"sessionRecent", {"&Recent", {}}},
    {"layout", {"&Layout", {"layoutMenuBar", "layoutStatusBar", nullptr, "windowMenu"}}},
    {"window", {"&Window", {"windowNew", "windowClone", nullptr, "windowClose", nullptr, "windowFullScreen", nullptr, "windowNext", "windowPrevious"}}},
    {"document", {"&Document", {"documentNew", "documentOpen", "documentRecentMenu", nullptr, "documentSave", "documentSaveAs", "documentSaveAll", nullptr, "documentClose", "documentCloseAll"}}},
    {"documentRecent", {"&Recent", {}}},
};
