#include "Camera.h"
#include "Engine.h"
#include "Player.h"
#include <SDL.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera(float x, float y)
    : Object(x, y, 32, 32)
{
    angle = 0.0f;          // Start pointing right
    angleSpeed = 0.25f;    // Degrees per frame (slower)
    minAngle = 0.0f;       // Starting angle
    maxAngle = 360.0f;     // Full rotation
    direction = 1.0f;      // Start rotating clockwise
    visionRange = 200.0f;  // 200 pixels range
    visionWidth = 60.0f;   // 60 degree cone
}

void Camera::update()
{
    // Rotate back and forth
    angle += angleSpeed * direction;
    
    // Change direction when hitting bounds
    if (angle >= maxAngle)
    {
        angle = maxAngle;
        direction = -1.0f;
    }
    else if (angle <= minAngle)
    {
        angle = minAngle;
        direction = 1.0f;
    }
}

void Camera::render()
{
    // Draw camera body (small black rectangle)
    Engine::drawRect(getX(), getY(), getWidth(), getHeight(), 50, 50, 50, 255);
    
    // Draw the vision cone (translucent red triangle)
    float cameraX = getX() + getWidth() / 2;
    float cameraY = getY() + getHeight() / 2;
    
    // Calculate cone vertices
    float angleRad = angle * M_PI / 180.0f;
    float halfWidthRad = (visionWidth / 2.0f) * M_PI / 180.0f;
    
    // Left edge of cone
    float leftAngle = angleRad - halfWidthRad;
    float leftX = cameraX + cos(leftAngle) * visionRange;
    float leftY = cameraY + sin(leftAngle) * visionRange;
    
    // Right edge of cone
    float rightAngle = angleRad + halfWidthRad;
    float rightX = cameraX + cos(rightAngle) * visionRange;
    float rightY = cameraY + sin(rightAngle) * visionRange;
    
    // Draw filled triangle using SDL rendering
    // We'll approximate with multiple lines to create a filled effect
    SDL_Renderer* renderer = Engine::E->getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Draw multiple lines from camera to arc to create filled cone
    int steps = 30;
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float currentAngle = leftAngle + (rightAngle - leftAngle) * t;
        float endX = cameraX + cos(currentAngle) * visionRange;
        float endY = cameraY + sin(currentAngle) * visionRange;
        
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 80); // Translucent red
        SDL_RenderDrawLine(renderer, (int)cameraX, (int)cameraY, (int)endX, (int)endY);
    }
}

bool Camera::isPointInCone(float px, float py)
{
    float cameraX = getX() + getWidth() / 2;
    float cameraY = getY() + getHeight() / 2;
    
    // Check distance first
    float dx = px - cameraX;
    float dy = py - cameraY;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > visionRange)
        return false;
    
    // Check if point is within the cone angle
    float pointAngle = atan2(dy, dx) * 180.0f / M_PI;
    
    // Normalize angles to 0-360
    while (pointAngle < 0) pointAngle += 360.0f;
    while (pointAngle >= 360.0f) pointAngle -= 360.0f;
    
    float normalizedCameraAngle = angle;
    while (normalizedCameraAngle < 0) normalizedCameraAngle += 360.0f;
    while (normalizedCameraAngle >= 360.0f) normalizedCameraAngle -= 360.0f;
    
    // Calculate angle difference
    float angleDiff = pointAngle - normalizedCameraAngle;
    while (angleDiff < -180.0f) angleDiff += 360.0f;
    while (angleDiff > 180.0f) angleDiff -= 360.0f;
    
    return fabs(angleDiff) <= (visionWidth / 2.0f);
}

bool Camera::checkPlayerInVision(Player* player)
{
    if (!player) return false;
    
    // Check corners of player bounding box
    float px = player->getX();
    float py = player->getY();
    float pw = player->getWidth();
    float ph = player->getHeight();
    
    // Check all four corners and center
    if (isPointInCone(px, py)) return true;
    if (isPointInCone(px + pw, py)) return true;
    if (isPointInCone(px, py + ph)) return true;
    if (isPointInCone(px + pw, py + ph)) return true;
    if (isPointInCone(px + pw/2, py + ph/2)) return true;
    
    return false;
}
