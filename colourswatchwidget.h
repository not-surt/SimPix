#ifndef COLOURSWATCHWIDGET_H
#define COLOURSWATCHWIDGET_H

#include <QAbstractButton>
#include <QFocusFrame>
#include "data.h"

class ColourSwatchWidget : public QAbstractButton
{
    Q_OBJECT

public:
    explicit ColourSwatchWidget(QWidget *parent = nullptr);
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

#endif // COLOURSWATCHWIDGET_H
