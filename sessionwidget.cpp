#include "sessionwidget.h"
#include "ui_sessionwidget.h"

SessionWidget::SessionWidget(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SessionWidget)
{
    ui->setupUi(this);
}

SessionWidget::~SessionWidget()
{
    delete ui;
}

void SessionWidget::setStringList(const QStringList &list)
{
    static_cast<SessionModel *>(ui->sessionView->model())->setList(list);
}
