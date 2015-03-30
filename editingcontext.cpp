#include "editingcontext.h"

EditingContext::EditingContext(QObject *parent) :
    QObject(parent), m_image(nullptr), m_palette(nullptr), m_colourSlots{0}, m_activeColourSlot(ColourSlot::Primary)
{
}

ImageData *EditingContext::image() const
{
    return m_image;
}

void EditingContext::setImage(ImageData *image)
{
    if (m_image != image) {
        m_image = image;
        emit changed(this);
    }
}

PaletteData *EditingContext::palette() const
{
    return m_palette;
}

void EditingContext::setPalette(PaletteData *palette)
{
    if (m_palette != palette) {
        m_palette = palette;
        emit changed(this);
    }
}


uint EditingContext::colourSlot(const ColourSlot colourSlot) const
{
    return m_colourSlots[colourSlot];
}

void EditingContext::setColourSlot(const uint colour, const ColourSlot colourSlot)
{
    if (m_colourSlots[colourSlot] != colour) {
        m_colourSlots[colourSlot] = colour;
        emit changed(this);
    }
}

EditingContext::ColourSlot EditingContext::activeColourSlot() const
{
    return m_activeColourSlot;
}

void EditingContext::setActiveColourSlot(const ColourSlot colourSlot)
{
    if (m_activeColourSlot != colourSlot) {
        m_activeColourSlot = colourSlot;
        emit changed(this);
    }
}
