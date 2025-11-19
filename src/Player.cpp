#include "Player.h"
#include "Engine.h"
#include "ImageDevice.h"
#include "InputDevice.h"
#include "Key.h"
#include "Door.h"
#include <SDL.h>
#include <iostream>
#include <string>

Player::Player(float x, float y)
    : Object(x, y, 100, 100) {
    std::cout << "Player created at (" << x << ", " << y << ")"<< std::endl;
}



// Use file-scope static animation state to avoid header/member issues
static int s_animationFrame = 0; // 0=rest, 1=left, 2=right, 3=down
static int s_lastMoveDirection = 0;

void Player::update() {
    s_lastMoveDirection = 0; // Reset to rest

    float wallThickness = 32.0f;
    float roomWidth = 800.0f;
    float roomHeight = 600.0f;

    if(InputDevice::isKeyDown(SDLK_LEFT)) {
        setX(getX()-2.5f);
        s_lastMoveDirection = 1; // Left frame
    }
    if(InputDevice::isKeyDown(SDLK_RIGHT)) {
        setX(getX()+2.5f);
        s_lastMoveDirection = 2; // Right frame
    }
    if(InputDevice::isKeyDown(SDLK_UP)) {
        setY(getY()-2.5f);
        s_lastMoveDirection = 3; // Up frame
    }
    if(InputDevice::isKeyDown(SDLK_DOWN)) {
        setY(getY()+2.5f);
        s_lastMoveDirection = 3; // Down frame
    }

    // Clamp player position to stay inside room boundaries
    if(getX() < wallThickness)
        setX(wallThickness);
    if(getX() + getWidth() > roomWidth - wallThickness)
        setX(roomWidth - wallThickness - getWidth());
    if(getY() < wallThickness)
        setY(wallThickness);
    if(getY() + getHeight() > roomHeight - wallThickness)
        setY(roomHeight - wallThickness - getHeight());

    s_animationFrame = s_lastMoveDirection;
}

void Player::render() {
    // Draw the pre-sliced frame texture (player0..player3)
    std::string tex = std::string("player") + std::to_string(s_animationFrame);
    Engine::drawImage(getX(), getY(), getWidth(), getHeight(), tex);
}

bool Player::checkCollision(float x1, float y1, float w1, float h1, 
                            float x2, float y2, float w2, float h2)
{
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

void Player::checkCollisionWithKey(Key* key)
{
    if (key && !key->isCollected())
    {
        if (checkCollision(getX(), getY(), getWidth(), getHeight(),
                          key->getX(), key->getY(), key->getWidth(), key->getHeight()))
        {
            key->collect();
            m_hasKey = true;
            std::cout << "Key collected!" << std::endl;
        }
    }
}

void Player::checkCollisionWithDoor(Door* door)
{
    if (door && !door->isOpened())
    {
        if (checkCollision(getX(), getY(), getWidth(), getHeight(),
                          door->getX(), door->getY(), door->getWidth(), door->getHeight()))
        {
            if (m_hasKey && door->isLocked())
            {
                door->unlock();
                door->open();
                
                if (door->isLevel2Door()) {
                    // Level 2 door - win the game
                    Engine::E->showGameOver();
                    std::cout << "Level 2 door opened! You Win!" << std::endl;
                } else {
                    // Level 1 door - transition to level 2
                    Engine::E->setLevel2();
                    m_hasKey = false; // Reset key for level 2
                    std::cout << "Entering Level 2!" << std::endl;
                }
            }
        }
    }
}
