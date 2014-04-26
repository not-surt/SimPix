#ifndef UTIL_H
#define UTIL_H

#include <QPixmap>

QPixmap *generateBackgroundPixmap(const uint size = 32);
extern QPixmap *canvasBackgroundPixmap;
extern QPixmap *swatchBackgroundPixmap;

#endif // UTIL_H
