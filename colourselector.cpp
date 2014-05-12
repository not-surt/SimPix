#include "colourselector.h"
#include "ui_colourselector.h"

#include "util.h"

ColourSelector::ColourSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColourSelector)
{
    ui->setupUi(this);

    QObject::connect(ui->redSpinBox, SIGNAL(valueChanged(const int)), this, SLOT(updateColour()));
    QObject::connect(ui->greenSpinBox, SIGNAL(valueChanged(const int)), this, SLOT(updateColour()));
    QObject::connect(ui->blueSpinBox, SIGNAL(valueChanged(const int)), this, SLOT(updateColour()));
    QObject::connect(ui->alphaSpinBox, SIGNAL(valueChanged(const int)), this, SLOT(updateColour()));
}

ColourSelector::~ColourSelector()
{
    delete ui;
}

void ColourSelector::setColour(const uint colour)
{
    if (m_colour.rgba() != colour) {
        m_colour.setRgba(colour);
        ObjectSignalBlocker redBlocker(ui->redSpinBox);
        ObjectSignalBlocker greenBlocker(ui->greenSpinBox);
        ObjectSignalBlocker blueBlocker(ui->blueSpinBox);
        ObjectSignalBlocker alphaBlocker(ui->alphaSpinBox);
        ui->redSpinBox->setValue(m_colour.red());
        ui->greenSpinBox->setValue(m_colour.green());
        ui->blueSpinBox->setValue(m_colour.blue());
        ui->alphaSpinBox->setValue(m_colour.alpha());
        emit colourChanged(colour);
    }
}

void ColourSelector::updateColour()
{
    QColor colour(ui->redSpinBox->value(), ui->greenSpinBox->value(), ui->blueSpinBox->value(), ui->alphaSpinBox->value());
    setColour(colour.rgba());
}

uint ColourSelector::colour()
{
    return m_colour.rgba();
}
