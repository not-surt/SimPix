#ifndef CANVAS_H
#define CANVAS_H

#include <QUndoCommand>
#include <QWidget>
#include <QPixmap>
#include <memory>
#include "image.h"

typedef struct Transform {
    QPointF pan;
    qreal zoom;
    QPointF pixelAspect;
    qreal rotation;
} Transform;

class Canvas : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_PROPERTY(bool tiled READ tiled WRITE setTiled NOTIFY tiledChanged)
    Q_ENUMS(image transform)

public:
    explicit Canvas(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    Image *image() const;
    Transform transform() const;
    bool tiled() const;

public slots:
    void setImage(Image *const image);
    void setTransform(const Transform &transform, const bool limit = true);
    void updateImage(const QRegion &region);

    void setTiled(bool tiled);

signals:
    void imageChanged(Image *image);
    void transformChanged(const Transform &transform);
    void tiledChanged(bool tiled);
    void clicked(const QPoint &position);
    void dragged(const QPoint &a, const QPoint &b);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    Image *m_image;
    Transform m_transform;
    bool m_tiled;
    QTransform matrix;
    QTransform inverseMatrix;
    QPoint lastMousePos;
    QPointF lastMouseImagePos;
    static std::unique_ptr<QPixmap> backgroundPattern;
    bool panKeyDown;

    void updateMatrix();
};

#endif // CANVAS_H
