#ifndef CANVASWINDOW_H
#define CANVASWINDOW_H

#include "openglwindow.h"
#include <QPixmap>
#include <memory>
#include "image.h"
#include "transform.h"

class QOpenGLContext;

class CanvasWindow : public OpenGLWindow
{
    Q_OBJECT
    Q_PROPERTY(Image *image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_PROPERTY(bool tiled READ tiled WRITE setTiled NOTIFY tiledChanged)
    Q_PROPERTY(bool showFrame READ showFrame WRITE setShowFrame NOTIFY showFrameChanged)
    Q_PROPERTY(bool showAlpha READ showAlpha WRITE setShowAlpha NOTIFY showAlphaChanged)
    Q_ENUMS(image transform)

public:
    CanvasWindow(QOpenGLContext *const shareContext = nullptr);
    virtual void initialize();
    virtual void render();
    Image *image() const;
    Transform transform() const;
    bool tiled() const;
    bool showFrame() const;
    bool showAlpha() const;
    QRect rect() const;
    const QMatrix4x4 &matrix();
    const QMatrix4x4 &inverseMatrix();
    GLuint vertexBuffer() const;

public slots:
    void setImage(Image *const image);
    void setTransform(const Transform &transform, const bool limit = true);
    void setTiled(const bool tiled);
    void setShowFrame(const bool showFrame);
    void setShowAlpha(bool showAlpha);
    void updateImage(const QRegion &region);

signals:
    void imageChanged(Image *image);
    void transformChanged(const Transform &transform);
    void tiledChanged(const bool tiled);
    void showFrameChanged(const bool showFrame);
    void showAlphaChanged(bool showAlpha);
    void clicked(const QPoint &position);
    void dragged(const QPoint &a, const QPoint &b);
    void mousePixelChanged(const QPoint &position, const uint colour, const int index);
    void mouseEntered();
    void mouseLeft();

protected:
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
    void update();
    void updateMatrix();

private:
    QOpenGLContext m_context;
    Image *m_image;
    Transform m_transform;
    bool m_tiled;
    QPoint lastMousePos;
    QVector3D lastMouseImagePos;
    bool panKeyDown;
    bool m_showFrame;
    bool m_showAlpha;
    QMatrix4x4 m_matrix;
    QMatrix4x4 m_inverseMatrix;
    bool matricesDirty;
    void updateMatrices();
    GLuint m_vertexBuffer;
};

#endif // CANVASWINDOW_H
