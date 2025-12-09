#include "BodyComponent.h"

BodyComponent::BodyComponent(float x, float y, float w, float h)
    : x_(x), y_(y), width_(w), height_(h) {
}

void BodyComponent::update(float dt) {
    x_ += velocityX_ * dt;
    y_ += velocityY_ * dt;
}
