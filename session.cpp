#include "session.h"

Session::Session(QObject *parent) :
    QObject(parent)
{
}

QList<Image *> Session::images() const
{
    return m_images;
}

QList<ImageEditor *> Session::editors() const
{
    return m_editors;
}
