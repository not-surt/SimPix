#ifndef UTIL_H
#define UTIL_H

#include <QPixmap>
#include <QSurface>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLWidget>

#include "application.h"

template<typename T>
inline T sign(const T value)
{
    return (value > 0) ? 1 : ((value < 0) ? -1 : 0);
}

template<typename T>
inline T fsel(const T a, const T b, const T c) {
  return a >= 0 ? b : c;
}

template<typename T>
inline T clamp(T value, const T min, const T max)
{
   value = fsel(value - min, value, min);
   return fsel(value - max, max, value);
}

template<typename T>
inline T wrap(T value, const T min, const T max)
{
    const T range = max - min;
    const T quotient = (value - min) / range;
    const T remainder = quotient - floor(quotient);
    return remainder * range + min;
}

class ObjectSignalBlocker
{
public:
    ObjectSignalBlocker(QObject *const object) :
        m_object(object), m_oldBlockSignalsState(object->blockSignals(true)) {}
    ~ObjectSignalBlocker() { m_object->blockSignals(m_oldBlockSignalsState); }

private:
    QObject *const m_object;
    const bool m_oldBlockSignalsState;
};

class ContextGrabber
{
public:
    ContextGrabber(QOpenGLContext *const context, QSurface *const surface) :
        ContextGrabber(context)
    {
        context->makeCurrent(surface);
    }
    ContextGrabber(QOpenGLWidget *const widget = APP->shareWidget()) :
        ContextGrabber(widget->context())
    {
        widget->makeCurrent();
    }
    ~ContextGrabber()
    {
        m_context->doneCurrent();
        if (m_oldContext) {
            m_oldContext->makeCurrent(m_oldSurface);
        }
    }

private:
    ContextGrabber(QOpenGLContext *const context) :
        m_context(context), m_oldContext(QOpenGLContext::currentContext()), m_oldSurface(m_oldContext ? m_oldContext->surface() : 0) {}
    QOpenGLContext *const m_context;
    QOpenGLContext *const m_oldContext;
    QSurface *const m_oldSurface;
};

QPixmap *generateBackgroundPixmap(const uint size = 32);
extern QPixmap *swatchBackgroundPixmap;
QString fileToString(QString fileName);
void qTransformFillGlslMat3(const QTransform &transform, GLfloat *const matrix);
void drawColourSwatch(QPainter *const painter, const QRect &rect, const QColor &colour);
QColor colourAdjustLightness(const QColor &colour, const int offset);

#endif // UTIL_H
