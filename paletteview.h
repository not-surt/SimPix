#ifndef PALETTEVIEW_H
#define PALETTEVIEW_H

#include <QListView>
#include <QStyledItemDelegate>
#include "image.h"

class PaletteView : public QListView
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage)
    Q_ENUMS(image)
public:
    explicit PaletteView(QWidget *parent = nullptr);
    Image *image() const;

signals:

public slots:
    void setImage(Image *image);


    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
};

#endif // PALETTEVIEW_H
