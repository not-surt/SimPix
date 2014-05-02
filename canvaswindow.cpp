#include "canvaswindow.h"

#include <QPainter>
#include <QDebug>

CanvasWindow::CanvasWindow()
{
}


void CanvasWindow::render(QPainter *painter)
{
    painter->fillRect(0, 0, width(), height(), QColor(rand() % 256, rand() % 256, rand() % 256));
}
