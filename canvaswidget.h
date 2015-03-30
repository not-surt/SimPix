#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <memory>
#include "scene.h"
#include "transform.h"
#include "editingcontext.h"

class CanvasWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
    Q_PROPERTY(Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_PROPERTY(bool tiled READ tiled WRITE setTiled NOTIFY tiledChanged)
    Q_PROPERTY(bool showFrame READ showFrame WRITE setShowFrame NOTIFY showFrameChanged)
    Q_PROPERTY(bool showAlpha READ showAlpha WRITE setShowAlpha NOTIFY showAlphaChanged)
    Q_ENUMS(scene transform)

public:
    explicit CanvasWidget(QWidget *parent = 0);    
    ~CanvasWidget();
    Scene *scene() const;
    const Transform &transform() const;
    bool tiled() const;
    bool showFrame() const;
    bool showAlpha() const;
    QRect rect() const;
    const QMatrix4x4 &matrix();
    const QMatrix4x4 &inverseMatrix();
    GLuint vertexBuffer() const;
    EditingContext &editingContext();

signals:
    void sceneChanged(Scene *scene);
    void transformChanged(const Transform &transform);
    void tiledChanged(const bool tiled);
    void showFrameChanged(const bool showFrame);
    void showAlphaChanged(bool showAlpha);
    void clicked(const QPoint &position, EditingContext *const editingContext);
    void dragged(const QPoint &a, const QPoint &b, EditingContext *const editingContext);
    void mousePixelChanged(const QPoint &position, const uint colour, const int index);
    void mouseEntered();
    void mouseLeft();

public slots:
    void setScene(Scene *const scene);
    void setTransform(const Transform &transform, const bool limit = true);
    void setTiled(const bool tiled);
    void setShowFrame(const bool showFrame);
    void setShowAlpha(bool showAlpha);

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
    Scene *m_scene;
    Transform m_transform;
    bool m_tiled;
    QPoint lastMousePos;
    QVector3D lastMouseScenePos;
    bool panKeyDown;
    bool m_showFrame;
    bool m_showAlpha;
    QMatrix4x4 m_matrix;
    QMatrix4x4 m_inverseMatrix;
    bool matricesDirty;
    void updateMatrices();
    GLuint m_vertexBuffer;
    QList<ImageData *>::iterator m_currentLayer;
    EditingContext m_editingContext;
};

#endif // CANVASWIDGET_H
