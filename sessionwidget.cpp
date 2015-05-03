#include "sessionwidget.h"
#include "ui_sessionwidget.h"

SessionWidget::SessionWidget(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SessionWidget), treeView(*new QTreeView(this))
{
    ui->setupUi(this);
    setCentralWidget(&treeView);
}

SessionWidget::~SessionWidget()
{
    delete ui;
}

void SessionWidget::setSession(Session *const session)
{
    treeView.setModel(session);
}
