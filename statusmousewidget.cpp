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

void StatusMouseWidget::setMouseInfo(const QPoint &position, const QColor colour, const int index)
{
    ui->mouseLabel->setText(QString("<b>X:</b>%1, <b>Y:</b>%2").arg(position.x()).arg(position.y()));
    if (colour.isValid()) {
        ui->colourSwatch->setColour(colour);
        ui->colourSwatch->show();
        QString text = QString("<b>R:</b>%1, <b>G:</b>%2, <b>B:</b>%3, <b>A:</b>%4")
                .arg(colour.red())
                .arg(colour.green())
                .arg(colour.blue())
                .arg(colour.alpha());
        if (index >= 0) {
            text += QString(", <b>Index:</b>%1").arg(index);
        }
        ui->colourLabel->setText(text);
        ui->colourLabel->show();
    }
    else {
        ui->colourSwatch->hide();
        ui->colourLabel->hide();
    }
}
