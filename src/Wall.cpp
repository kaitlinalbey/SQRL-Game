#include "Wall.h"
#include "Engine.h"

Wall::Wall(float x, float y, float w, float h)
    : Object(x, y, w, h)
{
}

void Wall::update()
{
    // Walls don't move - override to prevent random movement from Object base class
}

void Wall::render()
{
    // Draw stone texture tiled across the wall
    float tileSize = 32.0f; // Each stone block is 32x32 pixels
    float wallX = getX();
    float wallY = getY();
    float wallWidth = getWidth();
    float wallHeight = getHeight();
    
    for(float tileY = wallY; tileY < wallY + wallHeight; tileY += tileSize)
    {
        for(float tileX = wallX; tileX < wallX + wallWidth; tileX += tileSize)
        {
            float drawWidth = tileSize;
            float drawHeight = tileSize;
            
            // Clip the last tile if it goes beyond the wall boundaries
            if(tileX + tileSize > wallX + wallWidth)
                drawWidth = (wallX + wallWidth) - tileX;
            if(tileY + tileSize > wallY + wallHeight)
                drawHeight = (wallY + wallHeight) - tileY;
                
            Engine::drawImage(tileX, tileY, drawWidth, drawHeight, "stone block");
        }
    }
}
