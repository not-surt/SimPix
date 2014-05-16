#ifndef COLOURSELECTOR_H
#define COLOURSELECTOR_H

#include <QWidget>
#include <QColor>

namespace Ui {
class ColourSelector;
}

class ColourSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ColourSelector(QWidget *parent = nullptr);
    ~ColourSelector();
    void setColour(const QColor &colour);

signals:
    void colourChanged(const QColor &colour);

public slots:
    const QColor &colour();

private slots:
    void updateColour();

private:
    Ui::ColourSelector *ui;
    QColor m_colour;
};

#endif // COLOURSELECTOR_H
