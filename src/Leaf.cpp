#include "Leaf.h"

Leaf::Leaf(float x, float y, float width, float height, float speedX, float speedY)
    : x_(x), y_(y), width_(width), height_(height), speedX_(speedX), speedY_(speedY) {
}

void Leaf::update(float dt, int screenWidth, int screenHeight) {
    // Update position
    x_ += speedX_ * dt;
    y_ += speedY_ * dt;

    // Bounce off walls
    if (x_ < 0) {
        x_ = 0;
        speedX_ = -speedX_;
    }
    if (x_ + width_ > screenWidth) {
        x_ = screenWidth - width_;
        speedX_ = -speedX_;
    }

    // Keep leaf in bottom half and bounce off top/bottom boundaries
    if (y_ < screenHeight / 2) {
        y_ = screenHeight / 2;
        speedY_ = -speedY_;
    }
    if (y_ + height_ > screenHeight) {
        y_ = screenHeight - height_;
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
