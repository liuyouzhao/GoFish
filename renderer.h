#ifndef DESKRENDER_H
#define DESKRENDER_H

#include <QPainter>

class Renderer
{
public:
    Renderer();
    void render(QPainter *paint);
};

#endif // DESKRENDER_H
