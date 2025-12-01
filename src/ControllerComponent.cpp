#include "ControllerComponent.h"
#include "GameObject.h"
#include "BodyComponent.h"

ControllerComponent::ControllerComponent(float speed, int screenWidth)
    : speed_(speed), screenWidth_(screenWidth) {
}

void ControllerComponent::update(float dt) {
    auto* body = owner_->getComponent<BodyComponent>();
    if (!body) return;
    
    // Clamp to screen bounds
    if (body->getX() < 0) body->setX(0);
    if (body->getX() + body->getWidth() > screenWidth_) {
        body->setX(screenWidth_ - body->getWidth());
    }
}

void ControllerComponent::moveLeft(float dt) {
    auto* body = owner_->getComponent<BodyComponent>();
    if (body) {
        body->setX(body->getX() - speed_ * dt);
    }
}

void ControllerComponent::moveRight(float dt) {
    auto* body = owner_->getComponent<BodyComponent>();
    if (body) {
        body->setX(body->getX() + speed_ * dt);
    }
}
