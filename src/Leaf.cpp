#include "Leaf.h"

Leaf::Leaf(float x, float y, float width, float height, float speedX, float speedY)
    : x_(x), y_(y), width_(width), height_(height), speedX_(speedX), speedY_(speedY) {
}

void Leaf::update(float dt, int screenWidth, int screenHeight) {
    // Update position
    x_ += speedX_ * dt;
    y_ += speedY_ * dt;

    // Bounce off walls with some tolerance to go slightly off screen
    if (x_ < -20) {
        x_ = -20;
        speedX_ = -speedX_;
    }
    if (x_ + width_ > screenWidth + 20) {
        x_ = screenWidth + 20 - width_;
        speedX_ = -speedX_;
    }

    // Keep leaf in bottom half and bounce off top/bottom boundaries with tolerance
    if (y_ < screenHeight / 2 - 20) {
        y_ = screenHeight / 2 - 20;
        speedY_ = -speedY_;
    }
    if (y_ + height_ > screenHeight + 20) {
        y_ = screenHeight + 20 - height_;
        speedY_ = -speedY_;
    }
}

void Leaf::render(SDL_Renderer* renderer, SDL_Texture* texture) {
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
