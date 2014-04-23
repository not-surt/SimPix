#ifndef PALETTEPLUGIN_H
#define PALETTEPLUGIN_H

#include <QImageIOPlugin>

class PalettePlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.uninhabitant.SimPix.PalettePlugin" FILE "paletteplugin.json")
public:
    PalettePlugin();

    // QImageIOPlugin interface
public:
    virtual Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    virtual QImageIOHandler *create(QIODevice *device, const QByteArray &format) const;
};

#endif // PALETTEPLUGIN_H
