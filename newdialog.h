#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>

namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QSize imageSize READ imageSize)
    Q_PROPERTY(Mode mode READ mode)
    Q_PROPERTY(int palette READ palette)
    Q_ENUMS(size mode palette)

public:
    explicit NewDialog(QWidget *parent = 0);
    ~NewDialog();
    enum Mode {Indexed, RGB, RGBA};
    QSize imageSize() const;
    Mode mode() const;
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
