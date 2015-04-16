#include "palettewidget.h"
#include "colourswatchdelegate.h"
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include "application.h"
#include "util.h"
#include "ui_palettewidget.h"
#include <QColorDialog>
#include <QItemDelegate>
#include <QPainter>
#include <QStringListModel>
#include "data.h"

PaletteModel::PaletteModel(QObject *parent) :
    QAbstractListModel(parent), m_editingContext(nullptr)
{
}

int PaletteModel::rowCount(const QModelIndex &parent) const
{
    if (m_editingContext && m_editingContext->palette()) {
        return m_editingContext->palette()->length();
    }
    else {
        return 0;
    }
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "1:" << m_editingContext;
    if (!index.isValid()) {
        return QVariant();
    }

    if (!m_editingContext || !m_editingContext->palette() || (m_editingContext && ((index.row() < 0) || (index.row() >= (int)m_editingContext->palette()->length())))) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
//        qDebug() << "2:" << m_editingContext << m_editingContext->palette();
        QRgb colour;
        GLContextGrabber grab(APP->shareWidget());
        colour = m_editingContext->palette()->colour(index.row());
        return QColor(qRed(colour), qGreen(colour), qBlue(colour), qAlpha(colour));
    }
    else {
        return QVariant();
    }
}

QVariant PaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        return QString("Column %1").arg(section);
    }
    else {
        return QString("Row %1").arg(section);
    }
}

EditingContext *PaletteModel::editingContext() const
{
    return m_editingContext;
}

void PaletteModel::setEditingContext(EditingContext *editingContext)
{
    beginResetModel();
    m_editingContext = editingContext;
    endResetModel();
}


bool PaletteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_editingContext || !m_editingContext->palette() || (m_editingContext && ((index.row() < 0) || (index.row() >= (int)m_editingContext->palette()->length())))) {
        return false;
    }
    else {
        GLContextGrabber grab(APP->shareWidget());
        m_editingContext->palette()->setColour(index.row(), value.value<QColor>().rgba());
        emit dataChanged(index, index);
        return true;
    }
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index);
}

PaletteView::PaletteView(QWidget *parent) :
    QListView(parent)
{
    setModel(new PaletteModel(nullptr));
    setItemDelegate(new ColourSwatchDelegate);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(true);
    setResizeMode(QListView::Adjust);
    setSelectionRectVisible(false);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
}

EditingContext *PaletteView::editingContext() const
{
    return ((PaletteModel *)model())->editingContext();
}

void PaletteView::setEditingContext(EditingContext *editingContext)
{
    ((PaletteModel *)model())->setEditingContext(editingContext);
}


void PaletteView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
//        emit customContextMenuRequested(event->pos());
    }
    else
        QListView::mousePressEvent(event);
}

void PaletteView::mouseReleaseEvent(QMouseEvent *event)
{
    QListView::mouseReleaseEvent(event);
}

void PaletteView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QListView::mouseDoubleClickEvent(event);
}

void PaletteView::mouseMoveEvent(QMouseEvent *event)
{
    QListView::mouseMoveEvent(event);
}

PaletteWidget::PaletteWidget(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::PaletteWidget)
{
    ui->setupUi(this);
}

PaletteWidget::~PaletteWidget()
{
    delete ui;
}

EditingContext *PaletteWidget::editingContext() const
{
    return ui->paletteView->editingContext();
}

void PaletteWidget::setEditingContext(EditingContext *const editingContext)
{
    if (ui->paletteView->editingContext()) {
        QObject::disconnect(ui->paletteView, &PaletteView::clicked, this, &PaletteWidget::setColour);
        QObject::disconnect(ui->paletteView, &PaletteView::doubleClicked, this, &PaletteWidget::editColour);
    }
    ui->paletteView->setEditingContext(editingContext);
    if (editingContext) {
        QObject::connect(ui->paletteView, &PaletteView::clicked, this, &PaletteWidget::setColour);
        QObject::connect(ui->paletteView, &PaletteView::doubleClicked, this, &PaletteWidget::editColour);
    }
}

void PaletteWidget::editColour(QModelIndex index)
{
    if (index.isValid()) {
        const QColor colour = index.data().value<QColor>();
        const QColor result = QColorDialog::getColor(colour, this, QString(), QColorDialog::ShowAlphaChannel);
        if (result.isValid() && result != colour) {
            ui->paletteView->model()->setData(index, result);
            emit colourChanged(index.row());
            emit colourChanged();
        }
    }
}

void PaletteWidget::setColour(QModelIndex index)
{
    if (ui->paletteView->editingContext()) {
        Colour colour;
        colour.index = index.row();
        if (ui->paletteView->editingContext()->palette()) {
            QColor color = index.model()->data(index, Qt::DisplayRole).value<QColor>();
            colour.rgba = color.rgba();
        }
        ui->paletteView->editingContext()->setColourSlot(colour, ui->paletteView->editingContext()->activeColourSlot());
    }
}
