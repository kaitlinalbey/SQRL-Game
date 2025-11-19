#ifndef WALL_H
#define WALL_H

#include "Object.h"
#include <string>

class Wall : public Object
{
public:
    Wall(float x, float y, float w, float h);
    void update() override; // Override to prevent random movement
    void render() override;
};

#endif
