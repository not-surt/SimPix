#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "util.h"
#include "colourswatch.h"
#include "application.h"
#include "imageeditor.h"
#include "imagedocument.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QTextStream>
#include <QMdiSubWindow>
#include <QWidgetAction>
#include <QSpinBox>
#include "widgets.h"
#include "actions.h"

void SubWindow::closeEvent(QCloseEvent *event)
{
    ImageEditor *editor = static_cast<ImageEditor *>(widget());
    ImageDocument &image = static_cast<ImageDocument &>(editor->document);
    if (image.fileInfo.dirty()) {
        QString fileName = image.fileInfo.fileName().isNull() ? "<i>unnamed</>" : QFileInfo(image.fileInfo.fileName()).fileName();
        QMessageBox::StandardButton button = QMessageBox::question(this, QString(),
            QString(tr("The file \"<b>%1</b>\" has unsaved changes.<br/>"
                       "Do you want to save it before closing?")).arg(fileName),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);
        if (button == QMessageBox::Cancel) {
            event->ignore();
        }
        if (button == QMessageBox::Save) {
            if (!image.saveGui(this)) {
                event->ignore();
            }
        }
    }
}

QSize MdiArea::subWindowSizeOverhead() const
{
    QStyleOptionTitleBar optionTitleBar;
    optionTitleBar.titleBarState = 1;
    optionTitleBar.titleBarFlags = Qt::SubWindow;
    const int titleBarHeight = QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight, &optionTitleBar, nullptr);
    const int frameWidth = QApplication::style()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, nullptr, nullptr);
    return QSize(2 * frameWidth, titleBarHeight + frameWidth);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_mdi(nullptr), m_statusMouseWidget(nullptr), m_oldSubWindow(nullptr)
{    
    ui->setupUi(this);
    m_mdi = new MdiArea;
    m_mdi->setTabsClosable(true);
    m_mdi->setTabsMovable(true);
    m_mdi->setActivationOrder(QMdiArea::CreationOrder);
    setCentralWidget(m_mdi);
    setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

    // Copy actions to window so available when menu hidden. Is there a better way?
    QListIterator<QMenu *> menuIterator(ui->menuBar->findChildren<QMenu *>());
    while (menuIterator.hasNext()) {
        addActions(menuIterator.next()->findChildren<QAction *>());
//        // The better way? No.
//        QList<QAction *> actions = menuIterator.next()->findChildren<QAction *>();
//        QListIterator<QAction *> actionIterator(actions);
//        while (actionIterator.hasNext()) {
//            actionIterator.next()->setShortcutContext(Qt::ApplicationShortcut);
//        }
    }
    ui->menuBar->hide();

    QHash<QString, QActionGroup *> actionGroups;
    QHash<QString, QAction *> actions;
    actions["actionAbout"] = new QAction(this);

    QObject::connect(m_mdi, &MdiArea::subWindowActivated, [this](QMdiSubWindow *const subWindow) { this->activateSubWindow(static_cast<SubWindow *>(subWindow)); } );

    QObject::connect(ui->actionAbout, &QAction::triggered, APP, &Application::about);
    QObject::connect(ui->actionAboutQt, &QAction::triggered, APP, &Application::aboutQt);
    QObject::connect(ui->actionLicense, &QAction::triggered, APP, &Application::license);
    QObject::connect(ui->actionExit, &QAction::triggered, APP, &Application::closeAllWindows);

    QObject::connect(ui->actionFileNew, &QAction::triggered, this, &MainWindow::newImage);
    QObject::connect(ui->actionFileOpen, &QAction::triggered, this, &MainWindow::openImage);
    QObject::connect(ui->actionFileSave, &QAction::triggered, this, &MainWindow::saveImage);
    QObject::connect(ui->actionFileSaveAs, &QAction::triggered, this, &MainWindow::saveAsImage);
    QObject::connect(ui->actionFileClose, &QAction::triggered, this, &MainWindow::closeImage);

    QObject::connect(ui->actionMenu, &QAction::triggered, this->menuBar(), &QToolBar::setVisible);
    QObject::connect(ui->actionStatusBar, &QAction::triggered, this->statusBar(), &QToolBar::setVisible);
    QObject::connect(ui->actionFullscreen, &QAction::triggered, this, &MainWindow::setFullscreen);
    QObject::connect(ui->actionTileSubwindows, &QAction::triggered, m_mdi, &MdiArea::tileSubWindows);
    QObject::connect(ui->actionCascadeSubwindows, &QAction::triggered, m_mdi, &MdiArea::cascadeSubWindows);
    QObject::connect(ui->actionNextSubwindow, &QAction::triggered, m_mdi, &MdiArea::activateNextSubWindow);
    QObject::connect(ui->actionPreviousSubwindow, &QAction::triggered, m_mdi, &MdiArea::activatePreviousSubWindow);
    QObject::connect(ui->actionUseTabs, &QAction::triggered, this, &MainWindow::useTabs);
    QObject::connect(ui->actionToolbarsMenu, &QAction::triggered, this, &MainWindow::showToolbars);
    QObject::connect(ui->actionAllToolbars, &QAction::triggered, this, &MainWindow::showToolbars);
    QObject::connect(ui->actionLockToolbars, &QAction::triggered, this, &MainWindow::lockToolbars);
    QObject::connect(ui->actionDocksMenu, &QAction::triggered, this, &MainWindow::showDocks);
    QObject::connect(ui->actionAllDocks, &QAction::triggered, this, &MainWindow::showDocks);
    QObject::connect(ui->actionDockTitles, &QAction::triggered, this, &MainWindow::showDockTitles);
    QObject::connect(ui->actionLockDocks, &QAction::triggered, this, &MainWindow::lockDocks);

    QToolBar *toolBarBrush = ui->toolBarBrush;
    ModeActionGroup<EditingContext::BrushStyle> *brushStyleGroup = new ModeActionGroup<EditingContext::BrushStyle>(this);
    brushStyleGroup->addAction(ui->actionBrushStylePixel, EditingContext::BrushStyle::Pixel);
    brushStyleGroup->addAction(ui->actionBrushStyleRectangle, EditingContext::BrushStyle::Rectangle);
    brushStyleGroup->addAction(ui->actionBrushStyleEllipse, EditingContext::BrushStyle::Ellipse);
    ModeToolButtonAction<EditingContext::BrushStyle> *actionBrushStyle = new ModeToolButtonAction<EditingContext::BrushStyle>(*brushStyleGroup);
    actionBrushStyle->setMenu(ui->menuBrushStyle);
    toolBarBrush->addAction(actionBrushStyle);
    QObject::connect(brushStyleGroup, &QActionGroup::triggered, [brushStyleGroup](QAction *const action) { EditingContext::BrushStyle brushStyle = brushStyleGroup->mode(action); qDebug() << int(brushStyle); });

    ModeActionGroup<EditingContext::ToolSpace> *toolSpaceGroup = new ModeActionGroup<EditingContext::ToolSpace>(this);
    toolSpaceGroup->addAction(ui->actionToolSpaceImage, EditingContext::ToolSpace::Image);
    toolSpaceGroup->addAction(ui->actionToolSpaceImageAspectCorrected, EditingContext::ToolSpace::ImageAspectCorrected);
    toolSpaceGroup->addAction(ui->actionToolSpaceScreen, EditingContext::ToolSpace::Screen);
    toolSpaceGroup->addAction(ui->actionToolSpaceGrid, EditingContext::ToolSpace::Grid);
    ModeToolButtonAction<EditingContext::ToolSpace> *actionToolSpace = new ModeToolButtonAction<EditingContext::ToolSpace>(*toolSpaceGroup);
    actionToolSpace->setMenu(ui->menuToolSpace);
    toolBarBrush->addAction(actionToolSpace);
    QObject::connect(toolSpaceGroup, &QActionGroup::triggered, [toolSpaceGroup](QAction *const action) { EditingContext::ToolSpace toolSpace = toolSpaceGroup->mode(action); qDebug() << int(toolSpace); });

    toolBarBrush->addAction(ui->actionBrushPixelSnap);

    IntegerFieldAction *actionBrushWidth = new IntegerFieldAction();
    toolBarBrush->addAction(actionBrushWidth);
    IntegerFieldAction *actionBrushHeight = new IntegerFieldAction();
    toolBarBrush->addAction(actionBrushHeight);

    QToolBar *toolBarColour = ui->toolBarColour;
    ColourSwatchAction *actionPrimaryColour = new ColourSwatchAction();
    toolBarColour->addAction(actionPrimaryColour);
    ColourSwatchAction *actionSecondaryColour = new ColourSwatchAction();
    toolBarColour->addAction(actionSecondaryColour);
    ColourSwatchAction *actionBackgroundColour = new ColourSwatchAction();
    toolBarColour->addAction(actionBackgroundColour);
    toolBarColour->addAction(actionBrushHeight);

    QMenu *menuMenu = new QMenu;
    QListIterator<QMenu *> menu2(ui->menuBar->findChildren<QMenu *>(QString(), Qt::FindDirectChildrenOnly));
    while (menu2.hasNext()) {
        menuMenu->addMenu(menu2.next());
    }
    MenuToolButtonAction *menuToolButtonAction = new MenuToolButtonAction();
    menuToolButtonAction->setMenu(menuMenu);
    menuToolButtonAction->setText("Menu");
    ui->toolBarMain->insertAction(ui->toolBarMain->actions()[0], menuToolButtonAction);

    QMenu *toolBarMenu = new QMenu;
    ui->actionToolbars->setMenu(toolBarMenu);
    ui->actionToolbarsMenu->setMenu(toolBarMenu);
    static_cast<QToolButton *>(ui->toolBarLayout->widgetForAction(ui->actionToolbarsMenu))->setPopupMode(QToolButton::MenuButtonPopup);
    QListIterator<QToolBar *> toolbar(findChildren<QToolBar *>());
    while (toolbar.hasNext()) {
        toolBarMenu->addAction(toolbar.next()->toggleViewAction());
    }

    QMenu *dockMenu = new QMenu;
    ui->actionDocks->setMenu(dockMenu);
    ui->actionDocksMenu->setMenu(dockMenu);
    static_cast<QToolButton *>(ui->toolBarLayout->widgetForAction(ui->actionDocksMenu))->setPopupMode(QToolButton::MenuButtonPopup);
    QListIterator<QDockWidget *> dock(findChildren<QDockWidget *>());
    while (dock.hasNext()) {
        dockMenu->addAction(dock.next()->toggleViewAction());
    }

    m_statusMouseWidget = new StatusMouseWidget;
    m_statusMouseWidget->hide();
    statusBar()->addWidget(m_statusMouseWidget);
    statusBar()->setSizeGripEnabled(true);

    ui->toolBarLayout->hide();
    activateSubWindow(nullptr);

    APP->settings.beginGroup("window");
    restoreGeometry(APP->settings.value("geometry").toByteArray());
    restoreState(APP->settings.value("state").toByteArray());
    APP->settings.endGroup();
//    toolbars = findChildren<QToolBar *>();
//    toolbar = QListIterator<QToolBar *>(toolbars);
//    while (toolbar.hasNext()) {
//        toolbar.next()->restoreGeometry(APP->settings.value(QString("window/geometry").arg(toolbar.objectName())).toByteArray());
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

Editor *MainWindow::activeEditor()
{
    SubWindow *subWindow = static_cast<SubWindow *>(m_mdi->activeSubWindow());
    ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
    return editor;
}

void MainWindow::activateSubWindow(SubWindow *const subWindow)
{
    if (m_oldSubWindow) {
        QListIterator<QMetaObject::Connection> connectionsIterator(activeSubWindowConnections);
        while (connectionsIterator.hasNext()) {
            QMetaObject::Connection connection = (connectionsIterator.next());
            QObject::disconnect(connection);
        }
        activeSubWindowConnections.clear();
        ui->paletteWidget->setEditingContext(nullptr);
    }
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        ImageDocument &image = static_cast<ImageDocument &>(editor->document);

        activeSubWindowConnections.append(QObject::connect(&image.fileInfo, &FileInfo::dirtied, editor, SS_CAST(ImageEditor, update,)));
        activeSubWindowConnections.append(QObject::connect(&editor->editingContext(), &EditingContext::changed, [this](EditingContext *const context) { ui->colourContextWidget->setColourSlot(context->colourSlot(EditingContext::ColourSlot::Primary)); }));
        ui->paletteWidget->setEditingContext(&editor->editingContext());
        setWindowFilePath(image.fileInfo.fileName());

        activeSubWindowConnections.append(QObject::connect(ui->transformWidget, &TransformWidget::transformChanged, &editor->transform(), &Transform::copy));
        activeSubWindowConnections.append(QObject::connect(&editor->transform(), &Transform::changed, ui->transformWidget, &TransformWidget::setTransform));

        activeSubWindowConnections.append(QObject::connect(ui->actionWrap, &QAction::triggered, editor, &ImageEditor::setTiled));
        ui->actionWrap->setChecked(editor->tiled());
        activeSubWindowConnections.append(QObject::connect(ui->actionWrapX, &QAction::triggered, editor, &ImageEditor::setTileX));
        ui->actionWrapX->setChecked(editor->tileX());
        activeSubWindowConnections.append(QObject::connect(ui->actionWrapY, &QAction::triggered, editor, &ImageEditor::setTileY));
        ui->actionWrapY->setChecked(editor->tileY());
        activeSubWindowConnections.append(QObject::connect(ui->actionShowBounds, &QAction::triggered, editor, &ImageEditor::setShowBounds));
        ui->actionShowBounds->setChecked(editor->showBounds());
        activeSubWindowConnections.append(QObject::connect(ui->actionShowAlpha, &QAction::triggered, editor, &ImageEditor::setShowAlpha));
        ui->actionShowAlpha->setChecked(editor->showAlpha());
        activeSubWindowConnections.append(QObject::connect(ui->actionAntialias, &QAction::triggered, editor, &ImageEditor::setAntialias));
        ui->actionAntialias->setChecked(editor->antialias());

        activeSubWindowConnections.append(QObject::connect(editor, &ImageEditor::mouseEntered, m_statusMouseWidget, &StatusMouseWidget::show));
        activeSubWindowConnections.append(QObject::connect(editor, &ImageEditor::mouseLeft, m_statusMouseWidget, &StatusMouseWidget::hide));
        activeSubWindowConnections.append(QObject::connect(editor, &ImageEditor::mousePixelChanged, m_statusMouseWidget, &StatusMouseWidget::setMouseInfo));
    }
    else {
        ui->paletteWidget->setEditingContext(nullptr);
        setWindowFilePath(QString());
    }
    ui->actionWrap->setEnabled(subWindow);
    ui->actionWrapX->setEnabled(subWindow);
    ui->actionWrapY->setEnabled(subWindow);
    ui->actionShowBounds->setEnabled(subWindow);
    ui->actionShowAlpha->setEnabled(subWindow);
    ui->actionAntialias->setEnabled(subWindow);
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

void MainWindow::lockDocks(bool checked)
{
    QListIterator<QDockWidget *> dockIterator(findChildren<QDockWidget *>());
    while (dockIterator.hasNext()) {
        QDockWidget *dock = dockIterator.next();
        dock->setAllowedAreas(dock->isFloating() && checked ? Qt::NoDockWidgetArea : Qt::AllDockWidgetAreas);
        dock->setFeatures(dock->isFloating() || !checked ? (QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable) : QDockWidget::NoDockWidgetFeatures);
    }
}

void MainWindow::useTabs(bool checked)
{
    m_mdi->setViewMode(checked ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
}

void MainWindow::lockToolbars(bool checked)
{
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
    m_mdi->closeAllSubWindows();
    //if (!closeImage()) {
    if (m_mdi->subWindowList().length() > 0) {
//        event->ignore();
    }
    else {
        APP->settings.beginGroup("window");
        APP->settings.setValue("geometry", saveGeometry());
        APP->settings.setValue("state", saveState());
        APP->settings.endGroup();
    }
}

SubWindow *MainWindow::newEditorSubWindow(ImageEditor *const editor)
{
    ImageDocument &image = static_cast<ImageDocument &>(editor->document);
    SubWindow *subWindow = new SubWindow;
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->setWindowTitle(image.fileInfo.shortName());

    QSize overhead = m_mdi->subWindowSizeOverhead();
    QSize mdiAvailableSize = m_mdi->size() - overhead;
    const float scaleStep = 2;
    auto scaleToFit = [](float size, float available, float scaleStep) {
        return pow(scaleStep, floor(log(available / size) / log(scaleStep)));
    };
    const float scale = std::min(scaleToFit(image.imageData()->size.width(), mdiAvailableSize.width(), scaleStep),
                                 scaleToFit(image.imageData()->size.height(), mdiAvailableSize.height(), scaleStep));
    subWindow->resize(image.imageData()->size * scale + overhead);
    m_mdi->addSubWindow(subWindow);

    subWindow->setWidget(editor);
    editor->transform().setZoom(scale);
    editor->show();
    return subWindow;
}

void MainWindow::newImage()
{
    NewDialog *dialog = new NewDialog(this);
    if (dialog->exec()) {
        ImageDocument *image = new ImageDocument(APP->session, dialog->imageSize(), dialog->format());
//        APP->session.documents.append(image);
        ImageEditor *editor = static_cast<ImageEditor *>(image->createEditor());
        static_cast<SessionWidget *>(ui->dockWidgetSession->widget())->setSession(&APP->session);
        newEditorSubWindow(editor);
    }
}

void MainWindow::openImage()
{
    APP->settings.beginGroup("file");
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), APP->settings.value("lastOpened", QDir::homePath()).toString(), APP->fileDialogFilterString);
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
            static_cast<SessionWidget *>(ui->dockWidgetSession->widget())->setSession(&APP->session);
            newEditorSubWindow(editor);
        }
    }
    if (failed.length() > 0) {
        QMessageBox::critical(this, QString(), QString(tr("Error opening file(s) <b>\"%1\"</b>")).arg(failed.join(tr(", "))));
    }
    APP->settings.endGroup();
}

bool MainWindow::saveImage()
{
    SubWindow *subWindow = static_cast<SubWindow *>(m_mdi->activeSubWindow());
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        ImageDocument &image = static_cast<ImageDocument &>(editor->document);
        APP->settings.beginGroup("file");
        if (image.fileInfo.fileName().isNull()) {
            return saveAsImage();
        }
        if (image.save()) {
            APP->settings.setValue("lastSaved", image.fileInfo.fileName());
            return true;
        }
        else {
            QMessageBox::critical(this, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(image.fileInfo.fileName()).fileName()));
        }
        APP->settings.endGroup();
    }
    return false;
}

bool MainWindow::saveAsImage()
{
    SubWindow *subWindow = static_cast<SubWindow *>(m_mdi->activeSubWindow());
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
        fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), fileName, APP->fileDialogFilterString);
        if (!fileName.isNull()) {
            if (image.save(fileName)) {
                APP->settings.setValue("lastSaved", fileName);
            }
            else {
                QMessageBox::critical(this, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(fileName).fileName()));
            }
        }
        APP->settings.endGroup();
    }
    return false;
}

bool MainWindow::closeImage()
{
    SubWindow *subWindow = static_cast<SubWindow *>(m_mdi->activeSubWindow());
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        ImageDocument &image = static_cast<ImageDocument &>(editor->document);
        if (image.fileInfo.dirty()) {
            QString fileName = image.fileInfo.fileName().isNull() ? "<i>unnamed</>" : QFileInfo(image.fileInfo.fileName()).fileName();
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
//            m_mdi->closeActiveSubWindow();
            subWindow->close();
        }
//        m_mdi->closeActiveSubWindow();
        subWindow->close();
    }
    return true;
}

void MainWindow::setFullscreen(const bool fullscreen)
{
    if (fullscreen)
        showFullScreen();
    else
        showNormal();
}
