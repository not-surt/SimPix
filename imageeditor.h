#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <memory>
#include "document.h"
#include "editor.h"
#include "transform.h"
#include "editingcontext.h"

class ImageDocument;

class ImageEditor : public QOpenGLWidget, public Editor, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
    Q_PROPERTY(Transform transform READ transform)
    Q_PROPERTY(bool tiled READ tiled WRITE setTiled NOTIFY tiledChanged)
    Q_PROPERTY(bool tileX READ tileX WRITE setTileX NOTIFY tileXChanged)
    Q_PROPERTY(bool tileY READ tileY WRITE setTileY NOTIFY tileYChanged)
    Q_PROPERTY(bool showBounds READ showBounds WRITE setShowBounds NOTIFY showBoundsChanged)
    Q_PROPERTY(bool showAlpha READ showAlpha WRITE setShowAlpha NOTIFY showAlphaChanged)
    Q_PROPERTY(bool limitTransform READ limitTransform WRITE setLimitTransform NOTIFY limitTransformChanged)
    Q_ENUMS(image transform)

public:
    explicit ImageEditor(ImageDocument *image, QWidget *parent = nullptr);
    ~ImageEditor();
    ImageDocument *image() const { return m_image; }
    Transform &transform() { return m_transform; }
    bool tiled() const { return m_tiled; }
    bool tileX() const { return m_tileX; }
    bool tileY() const { return m_tileY; }
    bool showBounds() const { return m_showBounds; }
    bool showAlpha() const { return m_showAlpha; }
    QRect rect() const { return QRect(0, 0, width(), height()); }
    GLuint vertexBuffer() const { return m_vertexBuffer; }
    EditingContext &editingContext() { return m_editingContext; }
    bool limitTransform() const { return m_limitTransform; }
    void applyTransformLimits();    
    void updateTransform();

signals:
    void tiledChanged(const bool tiled);
    void tileXChanged(const bool tileX);
    void tileYChanged(const bool tileY);
    void showBoundsChanged(const bool showBounds);
    void showAlphaChanged(bool showAlpha);
    void mousePixelChanged(const QPoint &position, const QColor colour, const int index);
    void mouseEntered();
    void mouseLeft();
    void limitTransformChanged(bool arg);

public slots:
    void setTiled(const bool tiled) {
        if (m_tiled != tiled) {
            m_tiled = tiled;
            update();
            emit tiledChanged(tiled);
        }
    }
    void setTileX(const bool tileX) {
        if (m_tileX != tileX) {
            m_tileX = tileX;
            update();
            emit tiledChanged(tileX);
        }
    }
    void setTileY(const bool tileY) {
        if (m_tileY != tileY) {
            m_tileY = tileY;
            update();
            emit tiledChanged(tileY);
        }
    }
    void setShowBounds(const bool showBounds) {
        if (m_showBounds != showBounds) {
            m_showBounds = showBounds;
            update();
            emit showBoundsChanged(showBounds);
        }
    }
    void setShowAlpha(bool showAlpha) {
        if (m_showAlpha != showAlpha) {
            m_showAlpha = showAlpha;
            update();
            emit showAlphaChanged(showAlpha);
        }
    }
    void setLimitTransform(bool arg) {
        if (m_limitTransform != arg) {
            m_limitTransform = arg;
            emit limitTransformChanged(arg);
        }
    }

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
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
    ImageDocument *const m_image;
    Transform m_transform;
    bool m_tiled, m_tileX, m_tileY;
    QPoint mouseGrabPos;
    QPointF mouseGrabImagePos;
    QPoint mouseLastPos;
    QPointF mouseLastImagePos;
    bool panKeyDown;
    bool m_showBounds;
    bool m_showAlpha;
    QMatrix4x4 worldToClip;
    QMatrix4x4 viewToWorld;
    GLuint m_vertexBuffer;
    QList<ImageData *>::iterator m_currentLayer;
    EditingContext m_editingContext;
    bool m_limitTransform;
};

#endif // IMAGEEDITOR_H
