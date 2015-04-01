#ifndef EDITINGCONTEXT_H
#define EDITINGCONTEXT_H

#include <QObject>

class ImageData;
class PaletteData;

class EditingContext : public QObject
{
    Q_OBJECT
public:
    enum ColourSlot {
        Primary,
        Secondary,
        Eraser,
    };
    static const int COLOUR_SLOT_COUNT = 3;
    explicit EditingContext(QObject *parent = nullptr);
    ImageData *image() const;
    PaletteData *palette() const;
    uint colourSlot(const ColourSlot colourSlot) const;
    ColourSlot activeColourSlot() const;

signals:
    void changed(EditingContext *);

public slots:
    void setImage(ImageData *image);
    void setPalette(PaletteData *palette);
    void setColourSlot(const uint colour, const ColourSlot colourSlot = Primary);
    void setActiveColourSlot(const ColourSlot colourSlot);

protected:
    ImageData *m_image;
    PaletteData *m_palette;
    uint m_colourSlots[COLOUR_SLOT_COUNT];
    ColourSlot m_activeColourSlot;
};

#endif // EDITINGCONTEXT_H
