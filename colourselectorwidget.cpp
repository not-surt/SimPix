#include "colourselectorwidget.h"
#include "ui_colourselectorwidget.h"

#include "util.h"

ColourSelectorWidget::ColourSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColourSelectorWidget)
{
    ui->setupUi(this);

    auto func = [this](int i) { updateColour(); };
//    auto signal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    auto signal = SS_CAST(QSpinBox, valueChanged, int);
    QObject::connect(ui->redSpinBox, signal, func);
    QObject::connect(ui->greenSpinBox, signal, func);
    QObject::connect(ui->blueSpinBox, signal, func);
    QObject::connect(ui->alphaSpinBox, signal, func);
}

ColourSelectorWidget::~ColourSelectorWidget()
{
    delete ui;
}

void ColourSelectorWidget::setColour(const QColor &colour)
{
    if (m_colour != colour) {
        m_colour = colour;
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

void ColourSelectorWidget::updateColour()
{
    setColour(QColor(ui->redSpinBox->value(), ui->greenSpinBox->value(), ui->blueSpinBox->value(), ui->alphaSpinBox->value()));
}

const QColor &ColourSelectorWidget::colour()
{
    return m_colour;
}
