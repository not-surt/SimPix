#ifndef EDITINGCONTEXT_H
#define EDITINGCONTEXT_H

#include <QObject>
#include "data.h"

class EditingContext : public QObject
{
    Q_OBJECT
    Q_ENUMS(ColourSlot)
    Q_ENUMS(BrushStyle)
    Q_ENUMS(ToolSpace)
public:
    enum class ColourSlot {
        Primary,
        Secondary,
        Background,
    };
    enum class BrushStyle {
        Pixel,
        Rectangle,
        Ellipse,
    };
    enum class ToolSpace {
        Image,
        ImageAspectCorrected,
        Screen,
        Grid,
    };

    explicit EditingContext(QObject *parent = nullptr) :
        QObject(parent), m_image(nullptr), m_palette(nullptr), m_colourSlots{}, m_activeColourSlot(ColourSlot::Primary) {}
    ImageData *image() const { return m_image; }
    PaletteData *palette() const { return m_palette; }
    Colour colourSlot(const ColourSlot &slot) const { return m_colourSlots[static_cast<int>(slot)]; }
//    ColourSlot colourSlot(const int slot) const { return m_colourSlots[slot]; }
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
    void setColourSlot(const Colour &colour, const ColourSlot slot = ColourSlot::Primary) {
        if (m_colourSlots[static_cast<int>(slot)] != colour) {
            m_colourSlots[static_cast<int>(slot)] = colour;
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
    Colour m_colourSlots[COLOUR_SLOT_COUNT];
    ColourSlot m_activeColourSlot;

};

#endif // EDITINGCONTEXT_H
