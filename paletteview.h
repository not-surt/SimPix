#ifndef PALETTEVIEW_H
#define PALETTEVIEW_H

#include <QListView>
#include <QStyledItemDelegate>
#include "editingcontext.h"

class PaletteView : public QListView
{
    Q_OBJECT
public:
    explicit PaletteView(QWidget *parent = nullptr);
    EditingContext *editingContext() const;

signals:

public slots:
    void setEditingContext(EditingContext *editingContext);


    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
};

#endif // PALETTEVIEW_H
