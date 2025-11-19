#include "Squirrel.h"

Squirrel::Squirrel(float x, float y, float width, float height, float speed)
    : x_(x), y_(y), width_(width), height_(height), speed_(speed) {
}

void Squirrel::moveLeft(float dt) {
    x_ -= speed_ * dt;
}

void Squirrel::moveRight(float dt) {
    x_ += speed_ * dt;
}

void Squirrel::update(float dt, int screenWidth) {
    // Clamp position to screen bounds
    if (x_ < 0) x_ = 0;
    if (x_ + width_ > screenWidth) x_ = screenWidth - width_;
}

void Squirrel::render(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Rect rect = {
        static_cast<int>(x_),
        static_cast<int>(y_),
        static_cast<int>(width_),
        static_cast<int>(height_)
    };
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}
