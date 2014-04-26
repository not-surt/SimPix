#ifndef COLOURSWATCH_H
#define COLOURSWATCH_H

#include <QWidget>
#include <QColor>

class ColourSwatch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor colour READ colour WRITE setColour NOTIFY colourChanged)

public:
    explicit ColourSwatch(QWidget *parent = 0);
    virtual QSize sizeHint() const;
    const QColor &colour() const;

signals:
    void colourChanged(const QColor &arg);

public slots:
    void setColour(const QColor &arg);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QColor m_colour;
};

#endif // COLOURSWATCH_H
