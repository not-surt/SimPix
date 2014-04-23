#include "paletteplugin.h"

PalettePlugin::PalettePlugin()
{
}


QImageIOPlugin::Capabilities PalettePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
}

QImageIOHandler *PalettePlugin::create(QIODevice *device, const QByteArray &format) const
{
//    if (format.toLower() == "gpl")
//            return new MyStyle;
        return 0;
}
