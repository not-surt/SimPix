#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include "image.h"

#include <QDialog>

namespace Ui {
class NewDialog;
}

Q_DECLARE_METATYPE(QImage::Format)

class NewDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QSize imageSize READ imageSize)
    Q_PROPERTY(Image::Format mode READ mode)
    Q_PROPERTY(int palette READ palette)
    Q_ENUMS(size mode palette)

public:
    explicit NewDialog(QWidget *parent = 0);
    ~NewDialog();
    QSize imageSize() const;
    Image::Format mode() const;
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
