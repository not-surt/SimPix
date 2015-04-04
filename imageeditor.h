#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <memory>
#include "document.h"
#include "transform.h"
#include "editingcontext.h"

class ImageEditor : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_PROPERTY(bool tiled READ tiled WRITE setTiled NOTIFY tiledChanged)
    Q_PROPERTY(bool showBounds READ showBounds WRITE setShowBounds NOTIFY showBoundsChanged)
    Q_PROPERTY(bool showAlpha READ showAlpha WRITE setShowAlpha NOTIFY showAlphaChanged)
    Q_PROPERTY(bool limitTransform READ limitTransform WRITE setLimitTransform NOTIFY limitTransformChanged)
    Q_ENUMS(image transform)

public:
    explicit ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor();
    Image *image() const;
    const Transform &transform() const;
    bool tiled() const;
    bool showBounds() const;
    bool showAlpha() const;
    QRect rect() const;
    const QMatrix4x4 &matrix();
    const QMatrix4x4 &inverseMatrix();
    GLuint vertexBuffer() const;
    EditingContext &editingContext();
    bool limitTransform() const;

signals:
    void transformChanged(const Transform &transform);
    void tiledChanged(const bool tiled);
    void showBoundsChanged(const bool showBounds);
    void showAlphaChanged(bool showAlpha);
    void clicked(const QPoint &position, EditingContext *const editingContext);
    void dragged(const QPoint &a, const QPoint &b, EditingContext *const editingContext);
    void mousePixelChanged(const QPoint &position, const QColor colour, const int index);
    void mouseEntered();
    void mouseLeft();
    void limitTransformChanged(bool arg);

public slots:
    void setImage(Image *const image);
    void setTransform(const Transform &transform);
    void setTiled(const bool tiled);
    void setShowBounds(const bool showBounds);
    void setShowAlpha(bool showAlpha);
    void setLimitTransform(bool arg);

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
    void updateMatrix();

private:
    Image *m_image;
    Transform m_transform;
    bool m_tiled;
    QPoint lastMousePos;
    QVector3D lastMouseImagePos;
    bool panKeyDown;
    bool m_showBounds;
    bool m_showAlpha;
    QMatrix4x4 m_matrix;
    QMatrix4x4 m_inverseMatrix;
    bool matricesDirty;
    void updateMatrices();
    GLuint m_vertexBuffer;
    QList<ImageData *>::iterator m_currentLayer;
    EditingContext m_editingContext;
    bool m_limitTransform;
};

#endif // IMAGEEDITOR_H
