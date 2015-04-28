#include "settingsdialog.h"
#include "application.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeView>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{  
    QLayout *layout = new QVBoxLayout();
    QTreeView *treeView = new QTreeView();
    layout->addWidget(treeView);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Discard | QDialogButtonBox::Apply);
    layout->addWidget(buttonBox);
    setLayout(layout);
    QPushButton *discard = buttonBox->button(QDialogButtonBox::Discard);
    QObject::connect(discard, &QPushButton::clicked, this, &SettingsDialog::close);
    discard->setDefault(true);
}

void SettingsDialog::showEvent(QShowEvent *event)
{
    APP->settings.beginGroup("window/settings");
    restoreGeometry(APP->settings.value("geometry").toByteArray());
    APP->settings.endGroup();

    QDialog::showEvent(event);
}

void SettingsDialog::hideEvent(QHideEvent *event)
{
    APP->settings.beginGroup("window/settings");
    APP->settings.setValue("geometry", saveGeometry());
    APP->settings.endGroup();

    QDialog::hideEvent(event);
}
