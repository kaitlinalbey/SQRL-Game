#include "Key.h"
#include "Engine.h"

Key::Key(float x, float y)
    : Object(x, y, 32, 32), collected(false)
{
}

void Key::update()
{
    // Keys don't move
}

void Key::render()
{
    // Only render if not collected
    if (!collected)
    {
        // Draw a yellow rectangle as the key for now
        Engine::drawRect(getX(), getY(), getWidth(), getHeight(), 255, 215, 0, 255);
    }
}
