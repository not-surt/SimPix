#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
};

#endif // SETTINGSDIALOG_H
