#include "application.h"
#include "mainwindow.h"
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
    QApplication(argc, argv), shaders(), programs(), session(), actions(),
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
    createActions();
    setActionGroups();
    createMenus();
    setActionMenus();
    connectActions();

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

//MainWindow *Application::activeWindow()
//{
//    return nullptr;
//}

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

QMenuBar *Application::createMenuBar()
{
    return menuBarFromMenu(menus["main"]);
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
    MainWindow *window = new MainWindow;
    session.windows.append(window);
    window->show();
}

bool Application::windowClone()
{

}

bool Application::windowClose()
{
    MainWindow *window = dynamic_cast<MainWindow *>(activeWindow());
    window->close();
}

void Application::documentNew()
{
    MainWindow *window = dynamic_cast<MainWindow *>(activeWindow());
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
    MainWindow *window = dynamic_cast<MainWindow *>(activeWindow());
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
    MainWindow *window = dynamic_cast<MainWindow *>(activeWindow());
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
    MainWindow *window = dynamic_cast<MainWindow *>(activeWindow());
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
    MainWindow *window = dynamic_cast<MainWindow *>(activeWindow());
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

bool Application::editorNew()
{

}

bool Application::editorClone()
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

const QList<Application::ActionDefinition> Application::actionDefinitions = {
    {"applicationMenu", "&Application", nullptr, false, false, -1, nullptr, nullptr, "application", nullptr},
    {"applicationAbout", "&About", "help-about", false, false, -1, nullptr, "About application.", nullptr, nullptr},
    {"applicationAboutQt", "About &Qt", nullptr, false, false, -1, nullptr, "About Qt.", nullptr, nullptr},
    {"applicationLicense", "&License", nullptr, false, false, -1, nullptr, "Application license.", nullptr, nullptr},
    {"applicationSettings", "&Settings", nullptr, false, false, QKeySequence::Preferences, nullptr, "Application settings.", nullptr, nullptr},
    {"applicationExit", "&Exit", "application-exit", false, false, QKeySequence::Quit, nullptr, "Exit application.", nullptr, nullptr},

    {"sessionMenu", "&Session", nullptr, false, false, -1, nullptr, nullptr, "session", nullptr},
    {"sessionNew", "&New", "document-new", false, false, -1, nullptr, "Open new session.", nullptr, nullptr},
    {"sessionOpen", "&Open", "document-open", false, false, -1, nullptr, "Open existing session.", nullptr, nullptr},
    {"sessionRecentMenu", "&Recent", nullptr, false, false, -1, nullptr, nullptr, "sessionRecent", nullptr},
    {"sessionSave", "&Save", "document-save", false, false, -1, nullptr, "Save current session.", nullptr, nullptr},
    {"sessionSaveAs", "Save &As", "document-save-as", false, false, -1, nullptr, "Save current session as new filename.", nullptr, nullptr},
    {"sessionClose", "&Close", "document-close", false, false, -1, nullptr, "Close current session.", nullptr, nullptr},

    {"layoutMenu", "&Layout", nullptr, false, false, -1, nullptr, nullptr, "layout", nullptr},
    {"layoutFullScreen", "&Full Screen", "view-fullscreen", true, false, QKeySequence::FullScreen, nullptr, "Toggle window full screen.", nullptr, nullptr},
    {"layoutMenuBar", "&Menu Bar", nullptr, true, true, -1, "Ctrl+M", "Toggle menu bar visibility.", nullptr, nullptr},
    {"layoutStatusBar", "&Status Bar", nullptr, true, true, -1, "Ctrl+B", "Toggle status bar visibility.", nullptr, nullptr},

    {"windowsMenu", "&Windows", nullptr, false, false, -1, nullptr, nullptr, "windows", nullptr},
    {"windowsNext", "&Next Window", nullptr, false, false, -1, nullptr, "Switch to next window.", nullptr, nullptr},
    {"windowsPrevious", "&Previous Window", nullptr, false, false, -1, nullptr, "Switch to previous window.", nullptr, nullptr},

    {"subWindowsMenu", "&Subwindows", nullptr, false, false, -1, nullptr, nullptr, "subWindows", nullptr},
    {"subWindowsUseTabs", "&Use Tabs", nullptr, true, false, -1, nullptr, "Toggle tabbed subwindows.", nullptr, nullptr},
    {"subWindowsTile", "&Tile Subwindows", nullptr, false, false, -1, nullptr, "Tile arange subwindows.", nullptr, nullptr},
    {"subWindowsCascade", "&Cascade Subwindows", nullptr, false, false, -1, nullptr, "Cascade arange subwindows.", nullptr, nullptr},
    {"subWindowsNext", "&Next Subwindow", nullptr, false, false, -1, nullptr, "Switch to next subwindow.", nullptr, nullptr},
    {"subWindowsPrevious", "&Previous Subwindow", nullptr, false, false, -1, nullptr, "Switch to previous subwindow.", nullptr, nullptr},

    {"toolBarsMenu", "&Toolbars", nullptr, false, false, -1, nullptr, nullptr, "toolBars", nullptr},
    {"toolBarsLock", "&Lock Toolbars", nullptr, true, false, -1, nullptr, "Lock poisition of all toolbars.", nullptr, nullptr},
    {"toolBarsAll", "&All Toolbars", nullptr, true, false, -1, "Ctrl+Shift+T", "Toggle visibility of all toolbars.", nullptr, nullptr},

    {"docksMenu", "&Docks", nullptr, false, false, -1, nullptr, nullptr, "docks", nullptr},
    {"docksTitles", "&Dock Titles", nullptr, true, false, -1, nullptr, "Toggle visibility of dock titles.", nullptr, nullptr},
    {"docksLock", "&Lock Docks", nullptr, true, false, -1, nullptr, "Lock poisition of all docks.", nullptr, nullptr},
    {"docksAll", "&All Docks", nullptr, true, false, -1, "Ctrl+Shift+D", "Toggle visibility of all docks.", nullptr, nullptr},

    {"windowMenu", "&Window", nullptr, false, false, -1, nullptr, nullptr, "window", nullptr},
    {"windowNew", "&New Window", "window-new", false, false, -1, nullptr, "Open new window.", nullptr, nullptr},
    {"windowClone", "&Clone Window", nullptr, false, false, -1, nullptr, "Clone current window.", nullptr, nullptr},
    {"windowClose", "&Close Window", "window-close", false, false, -1, nullptr, "Close current window.", nullptr, nullptr},

    {"documentMenu", "&Document", nullptr, false, false, -1, nullptr, nullptr, "document", nullptr},
    {"documentNew", "&New", "document-new", false, false, QKeySequence::New, nullptr, "Open new document.", nullptr, nullptr},
    {"documentOpen", "&Open", "document-open", false, false, QKeySequence::Open, nullptr, "Open existing document.", nullptr, nullptr},
    {"documentRecentMenu", "&Recent", nullptr, false, false, -1, nullptr, nullptr, "documentRecent", nullptr},
    {"documentSave", "&Save", "document-save", false, false, QKeySequence::Save, nullptr, "Save current document.", nullptr, nullptr},
    {"documentSaveAs", "Save &As", "document-save-as", false, false, QKeySequence::SaveAs, nullptr, "Save current document as new filename.", nullptr, nullptr},
    {"documentSaveAll", "Save &All", nullptr, false, false, -1, nullptr, "Save All Documents", nullptr, nullptr},
    {"documentClose", "&Close", "document-close", false, false, -1, "Ctrl+Shift+W", "Close current document.", nullptr, nullptr},
    {"documentCloseAll", "Close &All", nullptr, false, false, -1, nullptr, "Close all documents.", nullptr, nullptr},

    {"editorMenu", "&Editor", nullptr, false, false, -1, nullptr, nullptr, "editor", nullptr},
    {"editorNew", "&New", "window-new", false, false, -1, "Ctrl+Shift+N", "Open new editor.", nullptr, nullptr},
    {"editorClone", "&Clone", nullptr, false, false, -1, "Ctrl+Shift+C", "Clone current editor.", nullptr, nullptr},
    {"editorClose", "&Close", "window-close", false, false, QKeySequence::Close, nullptr, "Close current editor.", nullptr, nullptr},
    {"editorCloseAll", "Close &All", nullptr, false, false, -1, nullptr, "Close all editors.", nullptr, nullptr},

    {"layerMenu", "&Layer", nullptr, false, false, -1, nullptr, nullptr, "layer", nullptr},
    {"layerWrap", "&Wrap", nullptr, true, false, -1, "Ctrl+Shift+W", "Toggle layer wrapping.", nullptr, nullptr},
    {"layerWrapX", "&Wrap X", nullptr, true, true, -1, nullptr, "Toggle layer X axis wrapping.", nullptr, nullptr},
    {"layerWrapY", "&Wrap Y", nullptr, true, true, -1, nullptr, "Toggle layer Y axis wrapping.", nullptr, nullptr},
    {"layerBounds", "&Bounds", nullptr, true, false, -1, "Ctrl+Shift+B", "Toggle layer bounds rendering.", nullptr, nullptr},
    {"layerAntialias", "&Antialias", nullptr, true, false, -1, nullptr, "Toggle layer antialiased rendering.", nullptr, nullptr},

    {"editMenu", "&Edit", nullptr, false, false, -1, nullptr, nullptr, "edit", nullptr},
    {"editUndo", "&Undo", "edit-undo", false, false, QKeySequence::Undo, nullptr, "Undo last edit.", nullptr, nullptr},
    {"editRedo", "&Redo", "edit-redo", false, false, QKeySequence::Redo, nullptr, "Redo last undo.", nullptr, nullptr},
    {"editCut", "Cu&t", "edit-cut", false, false, QKeySequence::Cut, nullptr, "Cut selection.", nullptr, nullptr},
    {"editCopy", "&Copy", "edit-copy", false, false, QKeySequence::Copy, nullptr, "Copy selection.", nullptr, nullptr},
    {"editPaste", "&Paste", "edit-paste", false, false, QKeySequence::Paste, nullptr, "Paste selection.", nullptr, nullptr},

    {"brushModeMenu", "&Brush Mode", nullptr, false, false, -1, nullptr, nullptr, "brushMode", nullptr},
    {"brushModePixel", "&Pixel", nullptr, true, false, -1, nullptr, "Pixel brush.", nullptr, "brushMode"},
    {"brushModeRectangle", "&Rectangle", nullptr, true, false, -1, nullptr, "Rectangle brush.", nullptr, "brushMode"},
    {"brushModeEllipse", "&Ellipse", nullptr, true, false, -1, nullptr, "Ellipse brush.", nullptr, "brushMode"},

    {"toolSpaceMenu", "&Tool Space", nullptr, false, false, -1, nullptr, nullptr, "toolSpace", nullptr},
    {"toolSpaceImage", "&Image Space", nullptr, true, false, -1, nullptr, "Image space.", nullptr, "toolSpace"},
    {"toolSpaceImageAspectCorrect", "&Image Space (Aspect Correct)", nullptr, true, false, -1, nullptr, "Aspect corrected image space.", nullptr, "toolSpace"},
    {"toolSpaceScreen", "&Screen Space", nullptr, true, false, -1, nullptr, "Screen space.", nullptr, "toolSpace"},
    {"toolSpaceGrid", "&Grid Space", nullptr, true, false, -1, nullptr, "Grid space.", nullptr, "toolSpace"},
};

void Application::createActions()
{
    for (int i = 0; i < Application::actionDefinitions.size(); i++) {
        const Application::ActionDefinition &definition = Application::actionDefinitions[i];
        QAction *action = new QAction(QIcon::fromTheme(definition.icon), definition.text, nullptr);
        action->setCheckable(definition.checkable);
        action->setChecked(definition.checked);
        if (definition.standardShortcut >= 0) {
            action->setShortcut(static_cast<enum QKeySequence::StandardKey>(definition.standardShortcut));
            action->setShortcutContext(Qt::ApplicationShortcut);
        }
        else if (!definition.customShortcut.isEmpty()) {
            action->setShortcut(QKeySequence(definition.customShortcut));
            action->setShortcutContext(Qt::ApplicationShortcut);
        }
        if (!definition.toolTip.isEmpty()) {
            action->setToolTip(definition.toolTip);
        }
        actions[definition.name] = action;
    }
//    qDebug() << actions;
}

const QList<Application::MenuDefinition> Application::menuDefinitions = {
    {"main", "&Menu", {"applicationMenu", "sessionMenu", "layoutMenu", "windowMenu", "documentMenu", "editorMenu", "layerMenu", "editMenu", "brushModeMenu", "toolSpaceMenu"}},
    {"application", "&Application", {"applicationAbout", "applicationAboutQt", "applicationLicense", nullptr, "applicationSettings", nullptr, "applicationExit"}},
    {"session", "&Session", {"sessionNew", "sessionOpen", "sessionRecentMenu", nullptr, "sessionSave", "sessionSaveAs", nullptr, "sessionClose"}},
    {"sessionRecent", "&Recent", {}},
    {"layout", "&Layout", {"layoutFullScreen", nullptr, "layoutMenuBar", "layoutStatusBar", nullptr, "windowsMenu", "subWindowsMenu", "toolBarsMenu", "docksMenu"}},
    {"windows", "&Windows", {"windowsNext", "windowsPrevious"}},
    {"subWindows", "&Subwindows", {"subWindowsUseTabs", nullptr, "subWindowsTile", "subWindowsCascade", nullptr, "subWindowsNext", "subWindowsPrevious"}},
    {"toolBars", "&Toolbars", {"toolBarsLock", nullptr, "toolBarsAll", nullptr}},
    {"docks", "&Docks", {"docksTitles", "docksLock", nullptr, "docksAll", nullptr}},
    {"window", "&Window", {"windowNew", "windowClone", nullptr, "windowClose"}},
    {"document", "&Document", {"documentNew", "documentOpen", "documentRecentMenu", nullptr, "documentSave", "documentSaveAs", "documentSaveAll", nullptr, "documentClose", "documentCloseAll"}},
    {"documentRecent", "&Recent", {}},
    {"editor", "&Editor", {"editorNew", "editorClone", nullptr, "editorClose", "editorCloseAll"}},
    {"layer", "&Layer", {"layerWrap", "layerWrapX", "layerWrapY", nullptr, "layerBounds", "layerAntialias"}},
    {"edit", "&Edit", {"editUndo", "editRedo", nullptr, "editCut", "editCopy", "editPaste"}},
    {"brushMode", "&Brush Mode", {"brushModePixel", "brushModeRectangle", "brushModeEllipse"}},
    {"toolSpace", "&Tool Space", {"toolSpaceImage", "toolSpaceImageAspectCorrect", "toolSpaceScreen", "toolSpaceGrid"}},
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

void Application::setActionGroups()
{
    for (int i = 0; i < Application::actionDefinitions.size(); i++) {
        const Application::ActionDefinition &definition = Application::actionDefinitions[i];
        if (!definition.groupName.isEmpty()) {
            if (!actionGroups[definition.groupName]) {
                actionGroups[definition.groupName] = new QActionGroup(this);
            }
            actions[definition.name]->setActionGroup(actionGroups[definition.groupName]);
        }
    }
}

void Application::connectActions()
{
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
}

QMenuBar *Application::menuBarFromMenu(QMenu *menu)
{
    QMenuBar *menuBar = new QMenuBar();
    QListIterator<QAction *> iterator(menu->actions());
    while (iterator.hasNext()) {
        QAction *const action = iterator.next();
        if (action->menu()) {
            menuBar->addMenu(action->menu());
        }
        else {
            menuBar->addAction(action);
        }
    }
    return menuBar;
}
