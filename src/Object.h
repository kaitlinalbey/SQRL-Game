#pragma once

class Object {
public:
    // Constructor with position and size parameters
    Object(float x, float y, float width, float height);
    virtual ~Object();
    
    // Core object methods
    virtual void update();
    virtual void render();

    float getX(){return x;}
    float getY(){return y;}
    float getWidth(){return width;}
    float getHeight(){return height;}
    void setX(float x){this->x = x;}
    void setY(float y){this->y = y;}
 private:   

    float x, y;        // Position
    float width, height; // Size
};
