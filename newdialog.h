#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include "data.h"
#include "ui_newdialog.h"

namespace Ui {
class NewDialog;
}

Q_DECLARE_METATYPE(QImage::Format)

class NewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewDialog(QWidget *parent = nullptr);
    ~NewDialog();
    QSize imageSize() const { return QSize(ui->widthSpinBox->value(), ui->heightSpinBox->value()); }
    TextureData::Format::Id format() const { return static_cast<TextureData::Format::Id>(ui->formatComboBox->currentData().toInt()); }
    int palette() const { return ui->formatComboBox->currentIndex(); }
protected:
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
private:
    Ui::NewDialog *ui;

public slots:
    void setPreset();
};

#endif // NEWDIALOG_H
