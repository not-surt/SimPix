#include "sessionwidget.h"
#include "ui_sessionwidget.h"

SessionWidget::SessionWidget(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SessionWidget), treeView(*new QTreeView(this))
{
    ui->setupUi(this);
    setCentralWidget(&treeView);
    TreeModel &treeModel = *new TreeModel();
    treeModel.setHeadings({"Type", "Name", "File Name", "Editors"});
    treeView.setModel(&treeModel);
}

SessionWidget::~SessionWidget()
{
    delete ui;
}

void SessionWidget::setSession(Session *const session)
{
    static_cast<TreeModel *>(treeView.model())->setRoot(session);
}
