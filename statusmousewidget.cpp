#include "statusmousewidget.h"
#include "ui_statusmousewidget.h"

StatusMouseWidget::StatusMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusMouseWidget)
{
    ui->setupUi(this);
}

StatusMouseWidget::~StatusMouseWidget()
{
    delete ui;
}

void StatusMouseWidget::setMouseInfo(const QPoint &position, const uint colour, const int index)
{
    ui->mouseLabel->setText(QString("<b>X:</b>%1, <b>Y:</b>%2").arg(position.x()).arg(position.y()));
    ui->colourSwatch->setColour(QColor(qRed(colour), qGreen(colour), qBlue(colour), qAlpha(colour)));
    ui->colourLabel->setText(QString("<b>R:</b>%1, <b>G:</b>%2, <b>B:</b>%3, <b>A:</b>%4%5")
                             .arg(qRed(colour))
                             .arg(qGreen(colour))
                             .arg(qBlue(colour))
                             .arg(qAlpha(colour))
                             .arg(index >= 0 ? QString(", <b>Index:</b>%1").arg(index) : ""));
}
