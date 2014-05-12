#ifndef UTIL_H
#define UTIL_H

#include <QPixmap>
#include <QOpenGLFunctions>

inline qreal fsel(const qreal a, const qreal b, const qreal c) {
  return a >= 0 ? b : c;
}

inline qreal clamp(qreal value, const qreal min, const qreal max)
{
   value = fsel(value - min, value, min);
   return fsel(value - max, max, value);
}

inline qreal wrap(qreal value, const qreal min, const qreal max)
{
    const qreal range = max - min;
    const qreal quotient = (value - min) / range;
    const qreal remainder = quotient - floor(quotient);
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

QPixmap *generateBackgroundPixmap(const uint size = 32);
extern QPixmap *canvasBackgroundPixmap;
extern QPixmap *swatchBackgroundPixmap;
QString fileToString(QString fileName);
void qTransformFillGlslMat3(const QTransform &transform, GLfloat *const matrix);

#endif // UTIL_H
