#include "colourselector.h"
#include "ui_colourselector.h"

ColourSelector::ColourSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColourSelector)
{
    ui->setupUi(this);
}

ColourSelector::~ColourSelector()
{
    delete ui;
}
