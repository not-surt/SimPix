#include "transform.h"

Transform::Transform(QObject *parent) :
    QObject(parent), m_pan(0., 0.), m_zoom(1.), m_pixelAspect(1., 1.), m_rotation(0.)
{
}
