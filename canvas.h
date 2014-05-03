#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPixmap>
#include <memory>
#include "image.h"
#include "transform.h"

class Canvas : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_PROPERTY(bool tiled READ tiled WRITE setTiled NOTIFY tiledChanged)
    Q_PROPERTY(bool showFrame READ showFrame WRITE setShowFrame NOTIFY showFrameChanged)
    Q_PROPERTY(bool showAlpha READ showAlpha WRITE setShowAlpha NOTIFY showAlphaChanged)
    Q_ENUMS(image transform)

public:
    explicit Canvas(QWidget *parent = nullptr);
    Image *image() const;
    Transform transform() const;
    bool tiled() const;
    bool showFrame() const;

    bool showAlpha() const;

public slots:
    void setImage(Image *const image);
    void setTransform(const Transform &transform, const bool limit = true);
    void updateImage(const QRegion &region);
    void setTiled(const bool tiled);
    void setShowFrame(const bool showFrame);
    void setShowAlpha(bool showAlpha);

signals:
    void imageChanged(Image *image);
    void transformChanged(const Transform &transform);
    void tiledChanged(const bool tiled);
    void clicked(const QPoint &position);
    void dragged(const QPoint &a, const QPoint &b);
    void showFrameChanged(const bool showFrame);
    void mouseEntered();
    void mouseLeft();
    void mousePixelChanged(const QPoint &position, const uint colour, const int index);
    void showAlphaChanged(bool showAlpha);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void tabletEvent(QTabletEvent *event);
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void keyReleaseEvent(QKeyEvent * event);
    virtual void enterEvent(QEvent *const event);
    virtual void leaveEvent(QEvent * const event);

private:
    Image *m_image;
    Transform m_transform;
    bool m_tiled;
    QPoint lastMousePos;
    QPointF lastMouseImagePos;
    bool panKeyDown;
    bool m_showFrame;
    bool m_showAlpha;
};

#endif // CANVAS_H
