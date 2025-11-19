#pragma once
#include <SDL.h>

class Squirrel {
public:
    Squirrel(float x, float y, float width, float height, float speed);
    
    void moveLeft(float dt);
    void moveRight(float dt);
    void update(float dt, int screenWidth);
    void render(SDL_Renderer* renderer, SDL_Texture* texture);
    
    float getX() const { return x_; }
    float getY() const { return y_; }
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }

private:
    float x_;
    float y_;
    float width_;
    float height_;
    float speed_;
};
