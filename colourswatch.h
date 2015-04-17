#ifndef COLOURSWATCH_H
#define COLOURSWATCH_H

#include <QAbstractButton>
#include <QFocusFrame>
#include "data.h"

class ColourSwatch : public QAbstractButton
{
    Q_OBJECT

public:
    explicit ColourSwatch(QWidget *parent = nullptr);
    virtual QSize sizeHint() const;
    const Colour &colour() const;

signals:
    void colourChanged(const Colour &colour);

public slots:
    void setColour(const Colour &colour);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    Colour m_colour;
    QFocusFrame *focusFrame;
};

#endif // COLOURSWATCH_H
