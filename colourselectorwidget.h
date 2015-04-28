#ifndef COLOURSELECTORWIDGET_H
#define COLOURSELECTORWIDGET_H

#include <QWidget>
#include <QColor>

namespace Ui {
class ColourSelectorWidget;
}

class ColourSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColourSelectorWidget(QWidget *parent = nullptr);
    ~ColourSelectorWidget();
    void setColour(const QColor &colour);

signals:
    void colourChanged(const QColor &colour);

public slots:
    const QColor &colour();

private slots:
    void updateColour();

private:
    Ui::ColourSelectorWidget *ui;
    QColor m_colour;
};

#endif // COLOURSELECTORWIDGET_H
