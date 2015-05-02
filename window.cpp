#include "window.h"
#include "newdialog.h"
#include "util.h"
#include "colourswatchwidget.h"
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
#include <QDockWidget>
#include <QToolBar>
#include <QStatusBar>
#include "widgets.h"
#include "palettewidget.h"
#include "colourcontextwidget.h"
#include "colourselectorwidget.h"
#include "sessionwidget.h"
#include "transformwidget.h"

SubWindow::SubWindow(QWidget *parent) :
    QMdiSubWindow(parent)
{
    // Hack to remove conflicting close shortcut
    systemMenu()->actions().last()->setShortcut(QKeySequence());
}

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

Window::Window(QWidget *parent) :
    QMainWindow(parent), ActionOwner(actionDefinitions, menuDefinitions, *APP), mdi(nullptr), statusMouseWidget(nullptr), oldSubWindow(nullptr)
{    
    mdi = new MdiArea;
    mdi->setTabsClosable(true);
    mdi->setTabsMovable(true);
    mdi->setActivationOrder(QMdiArea::CreationOrder);
    setCentralWidget(mdi);
    setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

    addActions(ActionOwner::actions.values());////////////////////

    QMenuBar *menuBar = new QMenuBar();
    QListIterator<QAction *> iterator(menus["main"]->actions());
    while (iterator.hasNext()) {
        QAction *const action = iterator.next();
        if (action->menu()) {
            menuBar->addMenu(action->menu());
        }
        else {
            menuBar->addAction(action);
        }
    }
    setMenuBar(menuBar);

//    // Copy actions to window so available when menu hidden. Is there a better way?
//    QListIterator<QMenu *> menuIterator(ui->menuBar->findChildren<QMenu *>());
//    while (menuIterator.hasNext()) {
//        addActions(menuIterator.next()->findChildren<QAction *>());
////        // The better way? No.
////        QList<QAction *> actions = menuIterator.next()->findChildren<QAction *>();
////        QListIterator<QAction *> actionIterator(actions);
////        while (actionIterator.hasNext()) {
////            actionIterator.next()->setShortcutContext(Qt::ApplicationShortcut);
////        }
//    }
//    ui->menuBar->hide();

    QObject::connect(mdi, &MdiArea::subWindowActivated, [this](QMdiSubWindow *const subWindow) { this->activateSubWindow(static_cast<SubWindow *>(subWindow)); } );

    QObject::connect(ActionOwner::actions["layoutFullScreen"], &QAction::triggered, this, &Window::setFullscreen);
    QObject::connect(ActionOwner::actions["layoutMenuBar"], &QAction::triggered, this->menuBar(), &QMenuBar::setVisible);
    QObject::connect(ActionOwner::actions["layoutStatusBar"], &QAction::triggered, this->statusBar(), &QStatusBar::setVisible);
    QObject::connect(ActionOwner::actions["subWindowsUseTabs"], &QAction::triggered, this, &Window::useTabs);
    QObject::connect(ActionOwner::actions["subWindowsTile"], &QAction::triggered, mdi, &MdiArea::tileSubWindows);
    QObject::connect(ActionOwner::actions["subWindowsCascade"], &QAction::triggered, mdi, &MdiArea::cascadeSubWindows);
    QObject::connect(ActionOwner::actions["subWindowsNext"], &QAction::triggered, mdi, &MdiArea::activateNextSubWindow);
    QObject::connect(ActionOwner::actions["subWindowsPrevious"], &QAction::triggered, mdi, &MdiArea::activatePreviousSubWindow);
    QObject::connect(ActionOwner::actions["toolBarsLock"], &QAction::triggered, this, &Window::lockToolbars);
    QObject::connect(ActionOwner::actions["toolBarsAll"], &QAction::triggered, this, &Window::showToolbars);
    QObject::connect(ActionOwner::actions["docksTitles"], &QAction::triggered, this, &Window::showDockTitles);
    QObject::connect(ActionOwner::actions["docksLock"], &QAction::triggered, this, &Window::lockDocks);
    QObject::connect(ActionOwner::actions["docksAll"], &QAction::triggered, this, &Window::showDocks);

    {
        QDockWidget *dock;

        dock = new QDockWidget();
        dock->setObjectName("dockPalette");
        dock->setWindowTitle("Palette");
        paletteWidget = new PaletteWidget();
        dock->setWidget(paletteWidget);
        addDockWidget(Qt::RightDockWidgetArea, dock);

        dock = new QDockWidget();
        dock->setObjectName("dockColourContext");
        dock->setWindowTitle("Colour Context");
        colourContextWidget = new ColourContextWidget();
        dock->setWidget(colourContextWidget);
        addDockWidget(Qt::RightDockWidgetArea, dock);

        dock = new QDockWidget();
        dock->setObjectName("dockColourSelector");
        dock->setWindowTitle("Colour Selector");
        colourSelector = new ColourSelectorWidget();
        dock->setWidget(colourSelector);
        addDockWidget(Qt::RightDockWidgetArea, dock);

        dock = new QDockWidget();
        dock->setObjectName("dockSession");
        dock->setWindowTitle("Session");
        sessionWidget = new SessionWidget();
        dock->setWidget(sessionWidget);
        addDockWidget(Qt::RightDockWidgetArea, dock);

        dock = new QDockWidget();
        dock->setObjectName("dockTransform");
        dock->setWindowTitle("Transform");
        transformWidget = new TransformWidget();
        dock->setWidget(transformWidget);
        addDockWidget(Qt::RightDockWidgetArea, dock);
    }

    {
        QToolBar *toolBar;

        toolBar = new QToolBar();
        toolBar->setObjectName("toolBarMain");
        toolBar->setWindowTitle("Main");
        MenuToolButtonAction *menuToolButtonAction = new MenuToolButtonAction();
        menuToolButtonAction->setText("Menu");
        menuToolButtonAction->setMenu(APP->menus["main"]);
        toolBar->addAction(menuToolButtonAction);
        toolBar->addSeparator();
        toolBar->addAction(ActionOwner::actions["documentNew"]);
        toolBar->addAction(ActionOwner::actions["documentOpen"]);
        toolBar->addSeparator();
        toolBar->addAction(ActionOwner::actions["documentSave"]);
        toolBar->addAction(ActionOwner::actions["documentSaveAs"]);
        toolBar->addSeparator();
        toolBar->addAction(ActionOwner::actions["documentClose"]);
        toolBar->addSeparator();
        toolBar->addAction(ActionOwner::actions["applicationExit"]);
        addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar = new QToolBar();
        toolBar->setObjectName("toolBarLayer");
        toolBar->setWindowTitle("Layer");
        toolBar->addAction(ActionOwner::actions["layerWrap"]);
        toolBar->addAction(ActionOwner::actions["layerWrapX"]);
        toolBar->addAction(ActionOwner::actions["layerWrapY"]);
        toolBar->addSeparator();
        toolBar->addAction(ActionOwner::actions["layerBounds"]);
        toolBar->addAction(ActionOwner::actions["layerAntialias"]);
        addToolBar(Qt::TopToolBarArea, toolBar);

//        toolBar = new QToolBar();
//        toolBar->setObjectName("toolBarEditingContext");
//        toolBar->setWindowTitle("Editing Context");
//        ModeActionGroup<EditingContext::BrushStyle> *brushStyleGroup = new ModeActionGroup<EditingContext::BrushStyle>(this);
//        brushStyleGroup->addAction(ui->actionBrushStylePixel, EditingContext::BrushStyle::Pixel);
//        brushStyleGroup->addAction(ui->actionBrushStyleRectangle, EditingContext::BrushStyle::Rectangle);
//        brushStyleGroup->addAction(ui->actionBrushStyleEllipse, EditingContext::BrushStyle::Ellipse);
//        ModeToolButtonAction<EditingContext::BrushStyle> *actionBrushStyle = new ModeToolButtonAction<EditingContext::BrushStyle>(*brushStyleGroup);
//        actionBrushStyle->setMenu(ui->menuBrushStyle);
//        toolBar->addAction(actionBrushStyle);
//        QObject::connect(brushStyleGroup, &QActionGroup::triggered, [brushStyleGroup](QAction *const action) { EditingContext::BrushStyle brushStyle = brushStyleGroup->mode(action); qDebug() << int(brushStyle); });
//        ModeActionGroup<EditingContext::ToolSpace> *toolSpaceGroup = new ModeActionGroup<EditingContext::ToolSpace>(this);
//        toolSpaceGroup->addAction(ui->actionToolSpaceImage, EditingContext::ToolSpace::Image);
//        toolSpaceGroup->addAction(ui->actionToolSpaceImageAspectCorrected, EditingContext::ToolSpace::ImageAspectCorrected);
//        toolSpaceGroup->addAction(ui->actionToolSpaceScreen, EditingContext::ToolSpace::Screen);
//        toolSpaceGroup->addAction(ui->actionToolSpaceGrid, EditingContext::ToolSpace::Grid);
//        ModeToolButtonAction<EditingContext::ToolSpace> *actionToolSpace = new ModeToolButtonAction<EditingContext::ToolSpace>(*toolSpaceGroup);
//        actionToolSpace->setMenu(ui->menuToolSpace);
//        toolBar->addAction(actionToolSpace);
//        QObject::connect(toolSpaceGroup, &QActionGroup::triggered, [toolSpaceGroup](QAction *const action) { EditingContext::ToolSpace toolSpace = toolSpaceGroup->mode(action); qDebug() << int(toolSpace); });
//        toolBar->addSeparator();
//        toolBar->addAction(ui->actionBrushPixelSnap);
//        IntegerFieldAction *actionBrushWidth = new IntegerFieldAction();
//        toolBar->addAction(actionBrushWidth);
//        IntegerFieldAction *actionBrushHeight = new IntegerFieldAction();
//        toolBar->addAction(actionBrushHeight);
//        ColourSwatchAction *actionPrimaryColour = new ColourSwatchAction();
//        toolBar->addAction(actionPrimaryColour);
//        ColourSwatchAction *actionSecondaryColour = new ColourSwatchAction();
//        toolBar->addAction(actionSecondaryColour);
//        ColourSwatchAction *actionBackgroundColour = new ColourSwatchAction();
//        toolBar->addAction(actionBackgroundColour);
//        toolBar->addAction(actionBrushHeight);
//        addToolBar(Qt::TopToolBarArea, toolBar);
    }

//    toolBarMenu = new QMenu;
//    ui->actionToolbars->setMenu(toolBarMenu);
//    ui->actionToolbarsMenu->setMenu(toolBarMenu);
//    static_cast<QToolButton *>(ui->toolBarLayout->widgetForAction(ui->actionToolbarsMenu))->setPopupMode(QToolButton::MenuButtonPopup);
//    QListIterator<QToolBar *> toolbar(findChildren<QToolBar *>());
//    while (toolbar.hasNext()) {
//        toolBarMenu->addAction(toolbar.next()->toggleViewAction());
//    }

//    dockMenu = new QMenu;
//    ui->actionDocks->setMenu(dockMenu);
//    ui->actionDocksMenu->setMenu(dockMenu);
//    static_cast<QToolButton *>(ui->toolBarLayout->widgetForAction(ui->actionDocksMenu))->setPopupMode(QToolButton::MenuButtonPopup);
//    QListIterator<QDockWidget *> dock(findChildren<QDockWidget *>());
//    while (dock.hasNext()) {
//        dockMenu->addAction(dock.next()->toggleViewAction());
//    }

    statusMouseWidget = new StatusMouseWidget;
    statusMouseWidget->hide();
    statusBar()->addWidget(statusMouseWidget);
    statusBar()->setSizeGripEnabled(true);

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

Window::~Window()
{
}

Editor *Window::activeEditor()
{
    SubWindow *subWindow = static_cast<SubWindow *>(mdi->activeSubWindow());
    ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
    return editor;
}

void Window::activateSubWindow(SubWindow *const subWindow)
{
    if (oldSubWindow) {
        QListIterator<QMetaObject::Connection> connectionsIterator(activeSubWindowConnections);
        while (connectionsIterator.hasNext()) {
            QMetaObject::Connection connection = (connectionsIterator.next());
            QObject::disconnect(connection);
        }
        activeSubWindowConnections.clear();
        paletteWidget->setEditingContext(nullptr);
    }
    if (subWindow) {
        ImageEditor *editor = static_cast<ImageEditor *>(subWindow->widget());
        ImageDocument &image = static_cast<ImageDocument &>(editor->document);

        activeSubWindowConnections.append(QObject::connect(&image.fileInfo, &FileInfo::dirtied, editor, SS_CAST(ImageEditor, update,)));
        activeSubWindowConnections.append(QObject::connect(&editor->editingContext(), &EditingContext::changed, [this](EditingContext *const context) { colourContextWidget->setColourSlot(context->colourSlot(EditingContext::ColourSlot::Primary)); }));
        paletteWidget->setEditingContext(&editor->editingContext());
        setWindowFilePath(image.fileInfo.fileName());

        activeSubWindowConnections.append(QObject::connect(transformWidget, &TransformWidget::transformChanged, &editor->transform(), &Transform::copy));
        activeSubWindowConnections.append(QObject::connect(&editor->transform(), &Transform::changed, transformWidget, &TransformWidget::setTransform));

//        activeSubWindowConnections.append(QObject::connect(ui->actionWrap, &QAction::triggered, editor, &ImageEditor::setTiled));
//        ui->actionWrap->setChecked(editor->tiled());
//        activeSubWindowConnections.append(QObject::connect(ui->actionWrapX, &QAction::triggered, editor, &ImageEditor::setTileX));
//        ui->actionWrapX->setChecked(editor->tileX());
//        activeSubWindowConnections.append(QObject::connect(ui->actionWrapY, &QAction::triggered, editor, &ImageEditor::setTileY));
//        ui->actionWrapY->setChecked(editor->tileY());
//        activeSubWindowConnections.append(QObject::connect(ui->actionShowBounds, &QAction::triggered, editor, &ImageEditor::setShowBounds));
//        ui->actionShowBounds->setChecked(editor->showBounds());
//        activeSubWindowConnections.append(QObject::connect(ui->actionShowAlpha, &QAction::triggered, editor, &ImageEditor::setShowAlpha));
//        ui->actionShowAlpha->setChecked(editor->showAlpha());
//        activeSubWindowConnections.append(QObject::connect(ui->actionAntialias, &QAction::triggered, editor, &ImageEditor::setAntialias));
//        ui->actionAntialias->setChecked(editor->antialias());

        activeSubWindowConnections.append(QObject::connect(editor, &ImageEditor::mouseEntered, statusMouseWidget, &StatusMouseWidget::show));
        activeSubWindowConnections.append(QObject::connect(editor, &ImageEditor::mouseLeft, statusMouseWidget, &StatusMouseWidget::hide));
        activeSubWindowConnections.append(QObject::connect(editor, &ImageEditor::mousePixelChanged, statusMouseWidget, &StatusMouseWidget::setMouseInfo));
    }
    else {
        paletteWidget->setEditingContext(nullptr);
        setWindowFilePath(QString());
    }
//    ui->actionWrap->setEnabled(subWindow);
//    ui->actionWrapX->setEnabled(subWindow);
//    ui->actionWrapY->setEnabled(subWindow);
//    ui->actionShowBounds->setEnabled(subWindow);
//    ui->actionShowAlpha->setEnabled(subWindow);
//    ui->actionAntialias->setEnabled(subWindow);
    oldSubWindow = subWindow;
}

void Window::showToolbars(bool checked)
{
//    QListIterator<QAction *> iterator(ui->actionToolbars->menu()->actions());
//    while (iterator.hasNext()) {
//        QAction *action = iterator.next();
//        if (action->isChecked() != checked) {
//            action->trigger();
//        }
//    }
}

void Window::showDocks(bool checked)
{
//    QListIterator<QAction *> iterator(ui->actionDocks->menu()->actions());
//    while (iterator.hasNext()) {
//        QAction *action = iterator.next();
//        if (action->isChecked() != checked) {
//            action->trigger();
//        }
//    }
}

void Window::showDockTitles(bool checked)
{
    QListIterator<QDockWidget *> iterator(findChildren<QDockWidget *>());
    while (iterator.hasNext()) {
        iterator.next()->setTitleBarWidget(checked ? nullptr : new QWidget);
    }
}

void Window::lockDocks(bool checked)
{
    QListIterator<QDockWidget *> dockIterator(findChildren<QDockWidget *>());
    while (dockIterator.hasNext()) {
        QDockWidget *dock = dockIterator.next();
        dock->setAllowedAreas(dock->isFloating() && checked ? Qt::NoDockWidgetArea : Qt::AllDockWidgetAreas);
        dock->setFeatures(dock->isFloating() || !checked ? (QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable) : QDockWidget::NoDockWidgetFeatures);
    }
}

void Window::useTabs(bool checked)
{
    mdi->setViewMode(checked ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
}

void Window::lockToolbars(bool checked)
{
    QListIterator<QToolBar *> toolBarIterator(findChildren<QToolBar *>());
    while (toolBarIterator.hasNext()) {
        QToolBar *toolBar = toolBarIterator.next();
        toolBar->setAllowedAreas(toolBar->isFloating() && checked ? Qt::NoToolBarArea : Qt::AllToolBarAreas);
        toolBar->setFloatable(toolBar->isFloating() || !checked);
        toolBar->setMovable(!checked);
    }
}

void Window::closeEvent(QCloseEvent *event)
{
    mdi->closeAllSubWindows();
    //if (!closeImage()) {
    if (mdi->subWindowList().length() > 0) {
//        event->ignore();
    }
    else {
        APP->settings.beginGroup("window");
        APP->settings.setValue("geometry", saveGeometry());
        APP->settings.setValue("state", saveState());
        APP->settings.endGroup();
    }
}

void Window::setFullscreen(const bool fullscreen)
{
    if (fullscreen)
        showFullScreen();
    else
        showNormal();
}

SubWindow *Window::newEditorSubWindow(ImageEditor *const editor)
{
    ImageDocument &image = static_cast<ImageDocument &>(editor->document);
    SubWindow *subWindow = new SubWindow;
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->setWindowTitle(image.fileInfo.shortName());

    QSize overhead = mdi->subWindowSizeOverhead();
    QSize mdiAvailableSize = mdi->size() - overhead;
    const float scaleStep = 2;
    auto scaleToFit = [](float size, float available, float scaleStep) {
        return pow(scaleStep, floor(log(available / size) / log(scaleStep)));
    };
    const float scale = std::min(scaleToFit(image.imageData()->size.width(), mdiAvailableSize.width(), scaleStep),
                                 scaleToFit(image.imageData()->size.height(), mdiAvailableSize.height(), scaleStep));
    subWindow->resize(image.imageData()->size * scale + overhead);
    mdi->addSubWindow(subWindow);

    subWindow->setWidget(editor);
    editor->transform().setZoom(scale);
    editor->show();
    return subWindow;
}

const QHash<QString, Window::ActionDefinition> Window::actionDefinitions = {
    {"subWindowsMenu", {"&Subwindows", nullptr, false, false, -1, nullptr, nullptr, "subWindows", nullptr}},
    {"subWindowsUseTabs", {"&Use Tabs", nullptr, true, false, -1, nullptr, "Toggle tabbed subwindows.", nullptr, nullptr}},
    {"subWindowsTile", {"&Tile Subwindows", nullptr, false, false, -1, nullptr, "Tile arange subwindows.", nullptr, nullptr}},
    {"subWindowsCascade", {"&Cascade Subwindows", nullptr, false, false, -1, nullptr, "Cascade arange subwindows.", nullptr, nullptr}},
    {"subWindowsNext", {"&Next Subwindow", nullptr, false, false, -1, nullptr, "Switch to next subwindow.", nullptr, nullptr}},
    {"subWindowsPrevious", {"&Previous Subwindow", nullptr, false, false, -1, nullptr, "Switch to previous subwindow.", nullptr, nullptr}},

    {"toolBarsMenu", {"&Toolbars", nullptr, false, false, -1, nullptr, nullptr, "toolBars", nullptr}},
    {"toolBarsLock", {"&Lock Toolbars", nullptr, true, false, -1, nullptr, "Lock poisition of all toolbars.", nullptr, nullptr}},
    {"toolBarsAll", {"&All Toolbars", nullptr, true, false, -1, "Ctrl+Shift+T", "Toggle visibility of all toolbars.", nullptr, nullptr}},

    {"docksMenu", {"&Docks", nullptr, false, false, -1, nullptr, nullptr, "docks", nullptr}},
    {"docksTitles", {"&Dock Titles", nullptr, true, false, -1, nullptr, "Toggle visibility of dock titles.", nullptr, nullptr}},
    {"docksLock", {"&Lock Docks", nullptr, true, false, -1, nullptr, "Lock poisition of all docks.", nullptr, nullptr}},
    {"docksAll", {"&All Docks", nullptr, true, false, -1, "Ctrl+Shift+D", "Toggle visibility of all docks.", nullptr, nullptr}},

    {"editorMenu", {"&Editor", nullptr, false, false, -1, nullptr, nullptr, "editor", nullptr}},
    {"editorNew", {"&New", "window-new", false, false, -1, "Ctrl+Shift+N", "Open new editor.", nullptr, nullptr}},
    {"editorClone", {"&Clone", nullptr, false, false, -1, "Ctrl+Shift+C", "Clone current editor.", nullptr, nullptr}},
    {"editorClose", {"&Close", "window-close", false, false, QKeySequence::Close, nullptr, "Close current editor.", nullptr, nullptr}},
    {"editorCloseAll", {"Close &All", nullptr, false, false, -1, nullptr, "Close all editors.", nullptr, nullptr}},

    {"layerMenu", {"&Layer", nullptr, false, false, -1, nullptr, nullptr, "layer", nullptr}},
    {"layerWrap", {"&Wrap", nullptr, true, false, -1, "Ctrl+Shift+W", "Toggle layer wrapping.", nullptr, nullptr}},
    {"layerWrapX", {"&Wrap X", nullptr, true, true, -1, nullptr, "Toggle layer X axis wrapping.", nullptr, nullptr}},
    {"layerWrapY", {"&Wrap Y", nullptr, true, true, -1, nullptr, "Toggle layer Y axis wrapping.", nullptr, nullptr}},
    {"layerBounds", {"&Bounds", nullptr, true, false, -1, "Ctrl+Shift+B", "Toggle layer bounds rendering.", nullptr, nullptr}},
    {"layerAntialias", {"&Antialias", nullptr, true, false, -1, nullptr, "Toggle layer antialiased rendering.", nullptr, nullptr}},

    {"editMenu", {"&Edit", nullptr, false, false, -1, nullptr, nullptr, "edit", nullptr}},
    {"editUndo", {"&Undo", "edit-undo", false, false, QKeySequence::Undo, nullptr, "Undo last edit.", nullptr, nullptr}},
    {"editRedo", {"&Redo", "edit-redo", false, false, QKeySequence::Redo, nullptr, "Redo last undo.", nullptr, nullptr}},
    {"editCut", {"Cu&t", "edit-cut", false, false, QKeySequence::Cut, nullptr, "Cut selection.", nullptr, nullptr}},
    {"editCopy", {"&Copy", "edit-copy", false, false, QKeySequence::Copy, nullptr, "Copy selection.", nullptr, nullptr}},
    {"editPaste", {"&Paste", "edit-paste", false, false, QKeySequence::Paste, nullptr, "Paste selection.", nullptr, nullptr}},

    {"brushModeMenu", {"&Brush Mode", nullptr, false, false, -1, nullptr, nullptr, "brushMode", nullptr}},
    {"brushModePixel", {"&Pixel", nullptr, true, false, -1, nullptr, "Pixel brush.", nullptr, "brushMode"}},
    {"brushModeRectangle", {"&Rectangle", nullptr, true, false, -1, nullptr, "Rectangle brush.", nullptr, "brushMode"}},
    {"brushModeEllipse", {"&Ellipse", nullptr, true, false, -1, nullptr, "Ellipse brush.", nullptr, "brushMode"}},

    {"toolSpaceMenu", {"&Tool Space", nullptr, false, false, -1, nullptr, nullptr, "toolSpace", nullptr}},
    {"toolSpaceImage", {"&Image Space", nullptr, true, false, -1, nullptr, "Image space.", nullptr, "toolSpace"}},
    {"toolSpaceImageAspectCorrect", {"&Image Space (Aspect Correct)", nullptr, true, false, -1, nullptr, "Aspect corrected image space.", nullptr, "toolSpace"}},
    {"toolSpaceScreen", {"&Screen Space", nullptr, true, false, -1, nullptr, "Screen space.", nullptr, "toolSpace"}},
    {"toolSpaceGrid", {"&Grid Space", nullptr, true, false, -1, nullptr, "Grid space.", nullptr, "toolSpace"}},
};

const QHash<QString, Window::MenuDefinition> Window::menuDefinitions = {
    {"main", {"&Menu", {"editorMenu", "layerMenu", "editMenu", "brushModeMenu", "toolSpaceMenu"}}},
    {"layout", {"&Layout", {"subWindowsMenu", "toolBarsMenu", "docksMenu"}}},
    {"subWindows", {"&Subwindows", {"subWindowsUseTabs", nullptr, "subWindowsTile", "subWindowsCascade", nullptr, "subWindowsNext", "subWindowsPrevious"}}},
    {"toolBars", {"&Toolbars", {"toolBarsLock", nullptr, "toolBarsAll", nullptr}}},
    {"docks", {"&Docks", {"docksTitles", "docksLock", nullptr, "docksAll", nullptr}}},
    {"editor", {"&Editor", {"editorNew", "editorClone", nullptr, "editorClose", "editorCloseAll"}}},
    {"layer", {"&Layer", {"layerWrap", "layerWrapX", "layerWrapY", nullptr, "layerBounds", "layerAntialias"}}},
    {"edit", {"&Edit", {"editUndo", "editRedo", nullptr, "editCut", "editCopy", "editPaste"}}},
    {"brushMode", {"&Brush Mode", {"brushModePixel", "brushModeRectangle", "brushModeEllipse"}}},
    {"toolSpace", {"&Tool Space", {"toolSpaceImage", "toolSpaceImageAspectCorrect", "toolSpaceScreen", "toolSpaceGrid"}}},
};