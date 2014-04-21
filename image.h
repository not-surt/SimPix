#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>

class Image : public QImage
{
public:
    explicit Image();
    explicit Image(const QSize &size, Format format);
    explicit Image(const QString &fileName, const char *format=0);

signals:

public slots:

};

#endif // IMAGE_H
