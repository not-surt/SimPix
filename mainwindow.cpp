#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "util.h"
#include "colourswatch.h"
#include "statusmousewidget.h"
#include "application.h"
#include "canvaswidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QTextStream>
#include <QMdiSubWindow>

const QString MainWindow::fileDialogFilterString = tr("PNG Image Files (*.png)");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_image(nullptr), m_canvas(nullptr), m_mdi(nullptr), m_images()
{    
    ui->setupUi(this);

    m_canvas = new CanvasWidget;
//    setCentralWidget(m_canvas);

    m_mdi = new QMdiArea;
    setCentralWidget(m_mdi);
    QMdiSubWindow *subWindow = new QMdiSubWindow;
    subWindow->setWidget(m_canvas);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    m_mdi->addSubWindow(subWindow);

    // Copy actions to window. Is there a better way?
    QListIterator<QMenu *> menu(ui->menuBar->findChildren<QMenu *>());
    while (menu.hasNext()) {
        addActions(menu.next()->findChildren<QAction *>());
    }

    QObject::connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newImage);
    QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openImage);
    QObject::connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveImage);
    QObject::connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAsImage);
    QObject::connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeScene);
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

    QObject::connect(ui->paletteWidget, &PaletteWidget::colourChanged, [&](const uint colour) { m_canvas->update(); });

    QObject::connect(ui->transformWidget, &TransformWidget::transformChanged, m_canvas, &CanvasWidget::setTransform);
    QObject::connect(m_canvas, &CanvasWidget::transformChanged, ui->transformWidget, &TransformWidget::setTransform);

    QObject::connect(ui->actionTiled, &QAction::triggered, m_canvas, &CanvasWidget::setTiled);
    QObject::connect(ui->actionShowFrame, &QAction::triggered, m_canvas, &CanvasWidget::setShowFrame);
    QObject::connect(ui->actionAlpha, &QAction::triggered, m_canvas, &CanvasWidget::setShowAlpha);

    QObject::connect(ui->actionToolbarsMenu, &QAction::triggered, this, &MainWindow::showToolbars);
    QObject::connect(ui->actionAllToolbars, &QAction::triggered, this, &MainWindow::showToolbars);
    QObject::connect(ui->actionDocksMenu, &QAction::triggered, this, &MainWindow::showDocks);
    QObject::connect(ui->actionAllDocks, &QAction::triggered, this, &MainWindow::showDocks);
    QObject::connect(ui->actionDockTitles, &QAction::triggered, this, &MainWindow::showDockTitles);
    QObject::connect(ui->actionLockSubwindows, &QAction::triggered, this, &MainWindow::lockSubwindows);

    QMenu *menuMenu = new QMenu(this);
    ui->actionMenuMenu->setMenu(menuMenu);
    static_cast<QToolButton *>(ui->windowToolBar->widgetForAction(ui->actionMenuMenu))->setPopupMode(QToolButton::InstantPopup);
    menuMenu->setTitle("&MenuMenu");
    QListIterator<QMenu *> menu2(ui->menuBar->findChildren<QMenu *>());
    while (menu2.hasNext()) {
        menuMenu->addMenu(menu2.next());
    }

    QMenu *toolBarMenu = new QMenu(this);
    ui->actionToolbars->setMenu(toolBarMenu);
    ui->actionToolbarsMenu->setMenu(toolBarMenu);
    static_cast<QToolButton *>(ui->windowToolBar->widgetForAction(ui->actionToolbarsMenu))->setPopupMode(QToolButton::MenuButtonPopup);
    QListIterator<QToolBar *> toolbar(findChildren<QToolBar *>());
    while (toolbar.hasNext()) {
        toolBarMenu->addAction(toolbar.next()->toggleViewAction());
    }

    QMenu *dockMenu = new QMenu(this);
    ui->actionDocks->setMenu(dockMenu);
    ui->actionDocksMenu->setMenu(dockMenu);
    static_cast<QToolButton *>(ui->windowToolBar->widgetForAction(ui->actionDocksMenu))->setPopupMode(QToolButton::MenuButtonPopup);
    QListIterator<QDockWidget *> dock(findChildren<QDockWidget *>());
    while (dock.hasNext()) {
        dockMenu->addAction(dock.next()->toggleViewAction());
    }

    StatusMouseWidget *statusMouseWidget = new StatusMouseWidget();
    statusMouseWidget->hide();
    statusBar()->addWidget(statusMouseWidget);
    QObject::connect(m_canvas, &CanvasWidget::mouseEntered, statusMouseWidget, &StatusMouseWidget::show);
    QObject::connect(m_canvas, &CanvasWidget::mouseLeft, statusMouseWidget, &StatusMouseWidget::hide);
    QObject::connect(m_canvas, &CanvasWidget::mousePixelChanged, statusMouseWidget, &StatusMouseWidget::setMouseInfo);
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

    m_canvas->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_image;
}

Image *MainWindow::image() const
{
    return m_image;
}

void MainWindow::setImage(Image *image)
{
    if (m_image != image) {
        auto updateSignal = static_cast<void (CanvasWidget::*)()>(&CanvasWidget::update);
        if (image) {
            QObject::connect(image, &Image::changed, m_canvas, updateSignal);
//            QObject::connect(m_canvas->editingContext(), SIGNAL(changed(EditingContext *)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::connect(m_canvas, &CanvasWidget::clicked, image, &Image::point);
            QObject::connect(m_canvas, &CanvasWidget::dragged, image, &Image::stroke);
            setWindowFilePath(image->fileName());
        }
        else {
            setWindowFilePath(QString());
        }
        emit sceneChanged(image);
        if (m_image) {
            QObject::disconnect(m_image, &Image::changed, m_canvas, updateSignal);
//            QObject::disconnect(m_canvas->editingContext(), SIGNAL(changed(EditingContext *)), ui->colourContextWidget, SLOT(setContextColour(const uint, const int)));
            QObject::disconnect(m_canvas, &CanvasWidget::clicked, m_image, &Image::point);
            QObject::disconnect(m_canvas, &CanvasWidget::dragged, m_image, &Image::stroke);
            m_image->deleteLater();
        }
        m_image = image;
        ui->paletteWidget->setEditingContext(nullptr);
        m_canvas->setScene(m_image);
        ui->paletteWidget->setEditingContext(&m_canvas->editingContext());
    }
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
        iterator.next()->setTitleBarWidget(checked ? nullptr : new QWidget());
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
    if (!closeScene()) {
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

bool MainWindow::newImage()
{
    if (!closeScene(false)) {
        return false;
    }
    NewDialog *dialog = new NewDialog(this);
    if (dialog->exec()) {
        qDebug() << "POOPIES!";
        CanvasWidget *canvas = new CanvasWidget;
        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(canvas);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        m_mdi->addSubWindow(subWindow);
        subWindow->show();
        subWindow->setFocus();
        Image *newImage = new Image(dialog->imageSize(), dialog->mode());
        m_images.append(newImage);
        setImage(newImage);
        return true;
    }
    return false;
}

bool MainWindow::openImage()
{
    if (!closeScene(false)) {
        return false;
    }
    QSettings settings;
    settings.beginGroup("file");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), settings.value("lastOpened", QDir::homePath()).toString(), fileDialogFilterString);
    if (!fileName.isNull()) {
        settings.setValue("lastOpened", fileName);
        Image *newImage = new Image(fileName);
        if (!newImage->imageData()) {
            delete newImage;
            QMessageBox::critical(this, QString(), QString(tr("Error opening file <b>\"%1\"</b>")).arg(QFileInfo(fileName).fileName()));
        }
        else {
            setImage(newImage);
            return true;
        }
    }
    settings.endGroup();
    return false;
}

bool MainWindow::saveImage()
{
    if (m_image) {
        QSettings settings;
        settings.beginGroup("file");
        if (m_image->fileName().isNull()) {
            return saveAsImage();
        }
        if (m_image->save()) {
            settings.setValue("lastSaved", m_image->fileName());
            return true;
        }
        else {
            QMessageBox::critical(this, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(m_image->fileName()).fileName()));
        }
        settings.endGroup();
    }
    return false;
}

bool MainWindow::saveAsImage()
{
    if (m_image) {
        QSettings settings;
        settings.beginGroup("file");
        QString fileName;
        if (!m_image->fileName().isNull()) {
            fileName = m_image->fileName();
        }
        else {
            QFileInfo fileInfo(settings.value("lastSaved", QDir::homePath()).toString());
            fileName = fileInfo.dir().path();
        }
        fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), fileName, fileDialogFilterString);
        if (!fileName.isNull()) {
            if (m_image->save(fileName)) {
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

bool MainWindow::closeScene(const bool doClose)
{
    if (m_image) {
        if (m_image->dirty()) {
            QString fileName = m_image->fileName().isNull() ? "<i>unnamed</>" : QFileInfo(m_image->fileName()).fileName();
            QMessageBox::StandardButton button = QMessageBox::question(this, QString(),
                                           QString(tr("The file \"<b>%1</b>\" has unsaved changes.<br/>"
                                                      "Do you want to save it before closing?")).arg(fileName),
                                           QMessageBox::Save | QMessageBox::Discard
                                           | QMessageBox::Cancel,
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
                setImage();
            }
        }
        else if (doClose) {
            setImage();
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
        text.arg(QCoreApplication::applicationName()).toLatin1());
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
