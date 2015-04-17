#include "statusmousewidget.h"
#include "ui_statusmousewidget.h"
#include <QDebug>

StatusMouseWidget::StatusMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusMouseWidget)
{
    ui->setupUi(this);
    setFixedHeight(ui->colourSwatch->height());
}

StatusMouseWidget::~StatusMouseWidget()
{
    delete ui;
}

void StatusMouseWidget::setMouseInfo(const QPoint &position, const Colour &colour)
{
    ui->mouseLabel->setText(QString("<b>X:</b>%1, <b>Y:</b>%2").arg(position.x()).arg(position.y()));
    ui->colourSwatch->setColour(colour);
    ui->colourSwatch->show();
    qDebug() << "ststus" << colour.bytes;
    QString text = QString("<b>R:</b>%1, <b>G:</b>%2, <b>B:</b>%3, <b>A:</b>%4")
            .arg(colour.r)
            .arg(colour.g)
            .arg(colour.b)
            .arg(colour.a);
    if (colour.index >= 0) {
        text += QString(", <b>Index:</b>%1").arg(colour.index);
    }
    ui->colourLabel->setText(text);
    ui->colourLabel->show();
}
