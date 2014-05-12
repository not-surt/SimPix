#ifndef PALETTEVIEW_H
#define PALETTEVIEW_H

#include <QListView>
#include <QStyledItemDelegate>
#include "scene.h"

class PaletteView : public QListView
{
    Q_OBJECT
    Q_PROPERTY(Scene *image READ image WRITE setImage)
    Q_ENUMS(image)
public:
    explicit PaletteView(QWidget *parent = nullptr);
    Scene *image() const;

signals:

public slots:
    void setImage(Scene *image);


    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
};

#endif // PALETTEVIEW_H
