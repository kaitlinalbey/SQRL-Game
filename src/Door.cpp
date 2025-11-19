#include "Door.h"
#include "Engine.h"

Door::Door(float x, float y, bool isLevel2Door)
    : Object(x, y, 64, 96), locked(true), opened(false), level2Door(isLevel2Door)
{
}

void Door::update()
{
    // Doors don't move
}

void Door::render()
{
    if (opened)
    {
        // Draw green when opened
        Engine::drawRect(getX(), getY(), getWidth(), getHeight(), 0, 255, 0, 255);
    }
    else if (locked)
    {
        // Draw brown/red when locked
        Engine::drawRect(getX(), getY(), getWidth(), getHeight(), 139, 69, 19, 255);
    }
    else
    {
        // Draw lighter brown when unlocked but not opened
        Engine::drawRect(getX(), getY(), getWidth(), getHeight(), 210, 180, 140, 255);
    }
}
