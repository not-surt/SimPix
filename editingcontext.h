#ifndef EDITINGCONTEXT_H
#define EDITINGCONTEXT_H

#include <QObject>

class ImageData;
class PaletteData;

class EditingContext : public QObject
{
    Q_OBJECT
    Q_ENUMS(ColourSlotId)
    Q_ENUMS(BrushStyle)
    Q_ENUMS(BrushSpace)
public:
    enum class ColourSlotId {
        Primary,
        Secondary,
        Background,
    };
    enum class BrushStyle {
        Pixel,
        Rectangle,
        Ellipse,
    };
    enum class BrushSpace {
        Image,
        ImageAspectCorrected,
        Screen,
        Grid,
    };
    struct ColourSlot {
        explicit ColourSlot(const uint p_rgba = 0, const int p_index = -1)
            : rgba(p_rgba), index(p_index){}
        inline bool operator==(const ColourSlot &rhs) { return this->rgba == rhs.rgba && this->index == rhs.index; }
        inline bool operator!=(const ColourSlot &rhs) { return !this->operator==(rhs); }

        uint rgba;
        short index;
    };

    explicit EditingContext(QObject *parent = nullptr) :
        QObject(parent), m_image(nullptr), m_palette(nullptr), m_colourSlots{}, m_activeColourSlot(ColourSlotId::Primary) {}
    ImageData *image() const { return m_image; }
    PaletteData *palette() const { return m_palette; }
    ColourSlot colourSlot(const ColourSlotId slot) const { return m_colourSlots[static_cast<int>(slot)]; }
//    ColourSlot colourSlot(const int slot) const { return m_colourSlots[slot]; }
    ColourSlotId activeColourSlot() const { return m_activeColourSlot; }

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
    void setColourSlot(const ColourSlot colour, const ColourSlotId slot = ColourSlotId::Primary) {
        if (m_colourSlots[static_cast<int>(slot)] != colour) {
            m_colourSlots[static_cast<int>(slot)] = colour;
            emit changed(this);
        }
    }
    void setActiveColourSlot(const ColourSlotId colourSlot) {
        if (m_activeColourSlot != colourSlot) {
            m_activeColourSlot = colourSlot;
            emit changed(this);
        }
    }

protected:
    static const int COLOUR_SLOT_COUNT = 3;
    ImageData *m_image;
    PaletteData *m_palette;
    ColourSlot m_colourSlots[COLOUR_SLOT_COUNT];
    ColourSlotId m_activeColourSlot;

};

#endif // EDITINGCONTEXT_H
