#include "Acorn.h"

Acorn::Acorn(float x, float y, float width, float height, float speed)
    : x_(x), y_(y), width_(width), height_(height), speed_(speed), active_(true) {
}

void Acorn::update(float dt) {
    if (active_) {
        y_ += speed_ * dt;
    }
}

void Acorn::render(SDL_Renderer* renderer, SDL_Texture* texture) {
    if (active_) {
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
}

bool Acorn::isOffScreen(int screenHeight) const {
    return y_ > screenHeight;
}
