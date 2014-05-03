#ifndef UTIL_H
#define UTIL_H

#include <QPixmap>

QPixmap *generateBackgroundPixmap(const uint size = 32);
extern QPixmap *canvasBackgroundPixmap;
extern QPixmap *swatchBackgroundPixmap;
QString fileToString(QString fileName);

#endif // UTIL_H
