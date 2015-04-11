#ifndef EDITINGCONTEXT_H
#define EDITINGCONTEXT_H

#include <QObject>

class ImageData;
class PaletteData;

class EditingContext : public QObject
{
    Q_OBJECT
    Q_ENUMS(ColourSlot)
    Q_ENUMS(BrushStyle)
    Q_ENUMS(BrushSpace)
public:
    enum ColourSlot {
        Primary,
        Secondary,
        Background,
    };
    enum BrushStyle {
        Pixel,
        Rectangle,
        Ellipse,
    };
    enum BrushSpace {
        Image,
        ImageAspectCorrected,
        Screen,
        Grid,
    };
    explicit EditingContext(QObject *parent = nullptr) :
        QObject(parent), m_image(nullptr), m_palette(nullptr), m_colourSlots{0}, m_activeColourSlot(ColourSlot::Primary) {}
    ImageData *image() const { return m_image; }
    PaletteData *palette() const { return m_palette; }
    uint colourSlot(const ColourSlot colourSlot) const { return m_colourSlots[colourSlot]; }
    ColourSlot activeColourSlot() const { return m_activeColourSlot; }

signals:
    void changed(EditingContext *);

public slots:
    void setImage(ImageData *image) {
        if (m_image != image) {
            m_image = image;
            emit changed(this);
        }
    }
    void setPalette(PaletteData *palette) {
        if (m_palette != palette) {
            m_palette = palette;
            emit changed(this);
        }
    }
    void setColourSlot(const uint colour, const ColourSlot colourSlot = Primary) {
        if (m_colourSlots[colourSlot] != colour) {
            m_colourSlots[colourSlot] = colour;
            emit changed(this);
        }
    }
    void setActiveColourSlot(const ColourSlot colourSlot) {
        if (m_activeColourSlot != colourSlot) {
            m_activeColourSlot = colourSlot;
            emit changed(this);
        }
    }

protected:
    static const int COLOUR_SLOT_COUNT = 3;
    ImageData *m_image;
    PaletteData *m_palette;
    uint m_colourSlots[COLOUR_SLOT_COUNT];
    ColourSlot m_activeColourSlot;

};

#endif // EDITINGCONTEXT_H
