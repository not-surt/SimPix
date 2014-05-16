#ifndef EDITINGCONTEXT_H
#define EDITINGCONTEXT_H

#include "scene.h"

#include <QObject>

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
    explicit EditingContext(QObject *parent = 0);

signals:

public slots:

private:
    Scene *m_scene;
    ImageData *m_layer;
    uint m_contextColours[COLOUR_SLOT_COUNT];
    ColourSlot m_activeContextColour;
};

#endif // EDITINGCONTEXT_H
