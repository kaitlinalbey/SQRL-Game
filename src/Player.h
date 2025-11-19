#pragma once
#include "Object.h"
#include <string>

class Key;
class Door;
class ColorBlock;

class Player : public Object {
public:
    // Constructor with position, size, and texture name
    Player(float x, float y);
    
    // Override Object methods
    virtual void update() override;
    virtual void render() override;
    
    // Collision and interaction methods
    void checkCollisionWithKey(Key* key);
    void checkCollisionWithDoor(Door* door);
    bool hasKey() const { return m_hasKey; }
    
    // Block carrying methods
    void setCarriedBlock(ColorBlock* block) { m_carriedBlock = block; }
    ColorBlock* getCarriedBlock() const { return m_carriedBlock; }

        private:
        int animationFrame = 0; // 0=rest, 1=left, 2=right, 3=down
        int lastMoveDirection = 0;
        bool m_hasKey = false;
        ColorBlock* m_carriedBlock = nullptr;
        
        // Helper to check if two rectangles overlap
        bool checkCollision(float x1, float y1, float w1, float h1, 
                           float x2, float y2, float w2, float h2);

};
