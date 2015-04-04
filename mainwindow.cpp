#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "util.h"
#include "colourswatch.h"
#include "application.h"
#include "imageeditor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QTextStream>
#include <QMdiSubWindow>

const QString MainWindow::fileDialogFilterString = tr("PNG Image Files (*.png)");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_mdi(nullptr),m_statusMouseWidget(nullptr), m_oldSubWindow(nullptr), m_images()
{    
    ui->setupUi(this);
    m_mdi = new QMdiArea;
    setCentralWidget(m_mdi);

    // Copy actions to window. Is there a better way?
    QListIterator<QMenu *> menu(ui->menuBar->findChildren<QMenu *>());
    while (menu.hasNext()) {
        addActions(menu.next()->findChildren<QAction *>());
    }
    ui->menuBar->hide();

    QObject::connect(m_mdi, &QMdiArea::subWindowActivated, this, &MainWindow::activateSubWindow);

    QObject::connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newImage);
    QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openImage);
    QObject::connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveImage);
    QObject::connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAsImage);
    QObject::connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeImage);
    QObject::connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    QObject::connect(ui->actionMenu, &QAction::triggered, this->menuBar(), &QToolBar::setVisible);
    QObject::connect(ui->actionStatusBar, &QAction::triggered, this->statusBar(), &QToolBar::setVisible);
    QObject::connect(ui->actionFullscreen, &QAction::triggered, this, &MainWindow::setFullscreen);
    QObject::connect(ui->actionTileSubwindows, &QAction::triggered, m_mdi, &QMdiArea::tileSubWindows);
    QObject::connect(ui->actionCascadeSubwindows, &QAction::triggered, m_mdi, &QMdiArea::cascadeSubWindows);
    QObject::connect(ui->actionNextSubwindow, &QAction::triggered, m_mdi, &QMdiArea::activateNextSubWindow);
    QObject::connect(ui->actionPreviousSubwindow, &QAction::triggered, m_mdi, &QMdiArea::activatePreviousSubWindow);
    QObject::connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    QObject::connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::aboutQt);
    QObject::connect(ui->actionLicense, &QAction::triggered, this, &MainWindow::license);

    QObject::connect(ui->actionToolbarsMenu, &QAction::triggered, this, &MainWindow::showToolbars);
    QObject::connect(ui->actionAllToolbars, &QAction::triggered, this, &MainWindow::showToolbars);
    QObject::connect(ui->actionDocksMenu, &QAction::triggered, this, &MainWindow::showDocks);
    QObject::connect(ui->actionAllDocks, &QAction::triggered, this, &MainWindow::showDocks);
    QObject::connect(ui->actionDockTitles, &QAction::triggered, this, &MainWindow::showDockTitles);
    QObject::connect(ui->actionLockSubwindows, &QAction::triggered, this, &MainWindow::lockSubwindows);

    QMenu *menuMenu = new QMenu;
    ui->actionMenuMenu->setMenu(menuMenu);
    static_cast<QToolButton *>(ui->mainToolBar->widgetForAction(ui->actionMenuMenu))->setPopupMode(QToolButton::InstantPopup);
    QListIterator<QMenu *> menu2(ui->menuBar->findChildren<QMenu *>());
    while (menu2.hasNext()) {
        menuMenu->addMenu(menu2.next());
    }

    QMenu *toolBarMenu = new QMenu;
    ui->actionToolbars->setMenu(toolBarMenu);
    ui->actionToolbarsMenu->setMenu(toolBarMenu);
    static_cast<QToolButton *>(ui->windowToolBar->widgetForAction(ui->actionToolbarsMenu))->setPopupMode(QToolButton::MenuButtonPopup);
    QListIterator<QToolBar *> toolbar(findChildren<QToolBar *>());
    while (toolbar.hasNext()) {
        toolBarMenu->addAction(toolbar.next()->toggleViewAction());
    }

    QMenu *dockMenu = new QMenu;
    ui->actionDocks->setMenu(dockMenu);
    ui->actionDocksMenu->setMenu(dockMenu);
    static_cast<QToolButton *>(ui->windowToolBar->widgetForAction(ui->actionDocksMenu))->setPopupMode(QToolButton::MenuButtonPopup);
    QListIterator<QDockWidget *> dock(findChildren<QDockWidget *>());
    while (dock.hasNext()) {
        dockMenu->addAction(dock.next()->toggleViewAction());
    }

    m_statusMouseWidget = new StatusMouseWidget;
    m_statusMouseWidget->hide();
    statusBar()->addWidget(m_statusMouseWidget);
    statusBar()->setSizeGripEnabled(true);

    QSettings settings;
    settings.beginGroup("window");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
//    toolbars = findChildren<QToolBar *>();
//    toolbar = QListIterator<QToolBar *>(toolbars);
//    while (toolbar.hasNext()) {
//        toolbar.next()->restoreGeometry(settings.value(QString("window/geometry").arg(toolbar.objectName())).toByteArray());
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::activateSubWindow(QMdiSubWindow *const subWindow) {
    if (m_oldSubWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(m_oldSubWindow->widget());
        Image *image = editor->image();

        if (image) {
            QObject::disconnect(image, &Image::changed, editor, SS_CAST(ImageEditor, update,));
    //            QObject::disconnect(m_editor->editingContext(), SIGNAL(changed(EditingContext *)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::disconnect(editor, &ImageEditor::clicked, image, &Image::point);
            QObject::disconnect(editor, &ImageEditor::dragged, image, &Image::stroke);
            ui->paletteWidget->setEditingContext(nullptr);
        }

        QObject::disconnect(ui->paletteWidget, SS_CAST(PaletteWidget, colourChanged,),
                         editor, SS_CAST(ImageEditor, update,));
        QObject::disconnect(ui->transformWidget, &TransformWidget::transformChanged, editor, &ImageEditor::setTransform);
        QObject::disconnect(editor, &ImageEditor::transformChanged, ui->transformWidget, &TransformWidget::setTransform);

        QObject::disconnect(ui->actionTiled, &QAction::triggered, editor, &ImageEditor::setTiled);
        QObject::disconnect(ui->actionShowFrame, &QAction::triggered, editor, &ImageEditor::setShowFrame);
        QObject::disconnect(ui->actionAlpha, &QAction::triggered, editor, &ImageEditor::setShowAlpha);

        QObject::disconnect(editor, &ImageEditor::mouseEntered, m_statusMouseWidget, &StatusMouseWidget::show);
        QObject::disconnect(editor, &ImageEditor::mouseLeft, m_statusMouseWidget, &StatusMouseWidget::hide);
        QObject::disconnect(editor, &ImageEditor::mousePixelChanged, m_statusMouseWidget, &StatusMouseWidget::setMouseInfo);
//        image->deleteLater();
    }
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        Image *image = editor->image();

        if (image) {
            QObject::connect(image, &Image::changed, editor, SS_CAST(ImageEditor, update,));
    //            QObject::connect(m_editor->editingContext(), SIGNAL(changed(EditingContext *)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::connect(editor, &ImageEditor::clicked, image, &Image::point);
            QObject::connect(editor, &ImageEditor::dragged, image, &Image::stroke);
            ui->paletteWidget->setEditingContext(&editor->editingContext());
            setWindowFilePath(image->fileName());
        }

        QObject::connect(ui->paletteWidget, SS_CAST(PaletteWidget, colourChanged,),
                         editor, SS_CAST(ImageEditor, update,));
        QObject::connect(ui->transformWidget, &TransformWidget::transformChanged, editor, &ImageEditor::setTransform);
        QObject::connect(editor, &ImageEditor::transformChanged, ui->transformWidget, &TransformWidget::setTransform);

        QObject::connect(ui->actionTiled, &QAction::triggered, editor, &ImageEditor::setTiled);
        QObject::connect(ui->actionShowFrame, &QAction::triggered, editor, &ImageEditor::setShowFrame);
        QObject::connect(ui->actionAlpha, &QAction::triggered, editor, &ImageEditor::setShowAlpha);

        QObject::connect(editor, &ImageEditor::mouseEntered, m_statusMouseWidget, &StatusMouseWidget::show);
        QObject::connect(editor, &ImageEditor::mouseLeft, m_statusMouseWidget, &StatusMouseWidget::hide);
        QObject::connect(editor, &ImageEditor::mousePixelChanged, m_statusMouseWidget, &StatusMouseWidget::setMouseInfo);
    }
    else {
        ui->paletteWidget->setEditingContext(nullptr);
        setWindowFilePath(QString());
    }
    m_oldSubWindow = subWindow;
}

void MainWindow::showToolbars(bool checked)
{
    QListIterator<QAction *> iterator(ui->actionToolbars->menu()->actions());
    while (iterator.hasNext()) {
        QAction *action = iterator.next();
        if (action->isChecked() != checked) {
            action->trigger();
        }
    }
}

void MainWindow::showDocks(bool checked)
{
    QListIterator<QAction *> iterator(ui->actionDocks->menu()->actions());
    while (iterator.hasNext()) {
        QAction *action = iterator.next();
        if (action->isChecked() != checked) {
            action->trigger();
        }
    }
}

void MainWindow::showDockTitles(bool checked)
{
    QListIterator<QDockWidget *> iterator(findChildren<QDockWidget *>());
    while (iterator.hasNext()) {
        iterator.next()->setTitleBarWidget(checked ? nullptr : new QWidget);
    }
}

void MainWindow::lockSubwindows(bool checked)
{
    QListIterator<QDockWidget *> dockIterator(findChildren<QDockWidget *>());
    while (dockIterator.hasNext()) {
        QDockWidget *dock = dockIterator.next();
        dock->setAllowedAreas(dock->isFloating() && checked ? Qt::NoDockWidgetArea : Qt::AllDockWidgetAreas);
        dock->setFeatures(dock->isFloating() || !checked ? (QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable) : QDockWidget::NoDockWidgetFeatures);
    }
    QListIterator<QToolBar *> toolBarIterator(findChildren<QToolBar *>());
    while (toolBarIterator.hasNext()) {
        QToolBar *toolBar = toolBarIterator.next();
        toolBar->setAllowedAreas(toolBar->isFloating() && checked ? Qt::NoToolBarArea : Qt::AllToolBarAreas);
        toolBar->setFloatable(toolBar->isFloating() || !checked);
        toolBar->setMovable(!checked);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!closeImage()) {
        event->ignore();
    }
    else {
        QSettings settings;
        settings.beginGroup("window");
        settings.setValue("geometry", saveGeometry());
        settings.setValue("state", saveState());
        settings.endGroup();
        QMainWindow::closeEvent(event);
    }
}

ImageEditor *MainWindow::newEditor(Image *const image) {
    QMdiSubWindow *subWindow = new QMdiSubWindow;
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->resize(200, 200);/////////////////////
    m_mdi->addSubWindow(subWindow);
    ImageEditor *editor = new ImageEditor;
    subWindow->setWidget(editor);
    editor->show();
    editor->setImage(image);
    return editor;
}

bool MainWindow::newImage()
{
    if (!closeImage(false)) {
        return false;
    }
    NewDialog *dialog = new NewDialog(this);
    if (dialog->exec()) {
        Image *newImage = new Image(dialog->imageSize(), dialog->mode());
        m_images.append(newImage);
        ImageEditor *editor = newEditor(newImage);
        return true;
    }
    return false;
}

bool MainWindow::openImage()
{
    if (!closeImage(false)) {
        return false;
    }
    QSettings settings;
    settings.beginGroup("file");
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), settings.value("lastOpened", QDir::homePath()).toString(), fileDialogFilterString);
    QStringListIterator fileNameIterator(fileNames);
    while (fileNameIterator.hasNext()) {
        QString fileName = fileNameIterator.next();
        settings.setValue("lastOpened", fileName);
        Image *image = new Image(fileName);
        if (!image->imageData()) {
            delete image;
            QMessageBox::critical(this, QString(), QString(tr("Error opening file <b>\"%1\"</b>")).arg(QFileInfo(fileName).fileName()));
        }
        else {
            m_images.append(image);
            ImageEditor *editor = newEditor(image);
            return true;
        }
    }
    settings.endGroup();
    return false;
}

bool MainWindow::saveImage()
{
    QMdiSubWindow *subWindow = m_mdi->activeSubWindow();
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        Image *image = editor->image();
        QSettings settings;
        settings.beginGroup("file");
        if (image->fileName().isNull()) {
            return saveAsImage();
        }
        if (image->save()) {
            settings.setValue("lastSaved", image->fileName());
            return true;
        }
        else {
            QMessageBox::critical(this, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(image->fileName()).fileName()));
        }
        settings.endGroup();
    }
    return false;
}

bool MainWindow::saveAsImage()
{
    QMdiSubWindow *subWindow = m_mdi->activeSubWindow();
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        Image *image = editor->image();
        QSettings settings;
        settings.beginGroup("file");
        QString fileName;
        if (!image->fileName().isNull()) {
            fileName = image->fileName();
        }
        else {
            QFileInfo fileInfo(settings.value("lastSaved", QDir::homePath()).toString());
            fileName = fileInfo.dir().path();
        }
        fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), fileName, fileDialogFilterString);
        if (!fileName.isNull()) {
            if (image->save(fileName)) {
                settings.setValue("lastSaved", fileName);
            }
            else {
                QMessageBox::critical(this, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(fileName).fileName()));
            }
        }
        settings.endGroup();
    }
    return false;
}

bool MainWindow::closeImage(const bool doClose)
{
    return true;/////////////////////////////////////////////////////////////////////////////
    QMdiSubWindow *subWindow = m_mdi->activeSubWindow();
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        Image *image = editor->image();
        if (image->dirty()) {
            QString fileName = image->fileName().isNull() ? "<i>unnamed</>" : QFileInfo(image->fileName()).fileName();
            QMessageBox::StandardButton button = QMessageBox::question(this, QString(),
                QString(tr("The file \"<b>%1</b>\" has unsaved changes.<br/>"
                           "Do you want to save it before closing?")).arg(fileName),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                QMessageBox::Save);
            if (button == QMessageBox::Cancel) {
                return false;
            }
            if (button == QMessageBox::Save) {
                if (!saveImage()) {
                    return false;
                }
            }
            if (doClose) {
                m_mdi->closeActiveSubWindow();
            }
        }
        else if (doClose) {
            m_mdi->closeActiveSubWindow();
        }
    }
    return true;
}

void MainWindow::setFullscreen(bool fullscreen)
{
    if (fullscreen)
        showFullScreen();
    else
        showNormal();
}

void MainWindow::about()
{
   QFile data(":/text/ABOUT");
   QString text;
   if (data.open(QFile::ReadOnly)) {
       text = QTextStream(&data).readAll();
   }
   QMessageBox::about(this, tr(QString("About %1").arg(QCoreApplication::applicationName()).toLatin1()),
        text.arg(QCoreApplication::applicationName()));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::license()
{
    QFile data(":/text/LICENSE");
    QString text;
    if (data.open(QFile::ReadOnly)) {
        text = QTextStream(&data).readAll();
    }
    QMessageBox::information(this, QString(), text);
}
