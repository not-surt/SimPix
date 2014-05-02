#ifndef CANVASWINDOW_H
#define CANVASWINDOW_H

#include "openglwindow.h"

class CanvasWindow : public OpenGLWindow
{
public:
    CanvasWindow();

public:
    virtual void render(QPainter *painter);

};

#endif // CANVASWINDOW_H
