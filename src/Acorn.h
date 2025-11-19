#pragma once
#include <SDL.h>

class Acorn {
public:
    Acorn(float x, float y, float width, float height, float speed);
    
    void update(float dt);
    void render(SDL_Renderer* renderer, SDL_Texture* texture);
    
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }
    
    float getX() const { return x_; }
    float getY() const { return y_; }
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    
    bool isOffScreen(int screenHeight) const;

private:
    float x_;
    float y_;
    float width_;
    float height_;
    float speed_;
    bool active_;
};
