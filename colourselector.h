#ifndef COLOURSELECTOR_H
#define COLOURSELECTOR_H

#include <QWidget>

namespace Ui {
class ColourSelector;
}

class ColourSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ColourSelector(QWidget *parent = 0);
    ~ColourSelector();

private:
    Ui::ColourSelector *ui;
};

#endif // COLOURSELECTOR_H
