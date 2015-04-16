#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include "document.h"

#include <QDialog>

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
    QSize imageSize() const;
    TextureDataFormat::Id mode() const;
    int palette() const;
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::NewDialog *ui;

    // QDialog interface
public slots:
    virtual void accept();
    void setPreset();
};

#endif // NEWDIALOG_H
