#include "image.h"

Image::Image() :
    QImage()
{
}

Image::Image(const QSize &size, QImage::Format format) :
    QImage(size, format)
{

}

Image::Image(const QString &fileName, const char *format) :
    QImage(fileName, format)
{

}
