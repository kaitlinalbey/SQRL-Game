#include "BehaviorComponent.h"
#include "GameObject.h"
#include "BodyComponent.h"

// BounceBehavior implementation
BounceBehavior::BounceBehavior(int screenWidth, int screenHeight)
    : screenWidth_(screenWidth), screenHeight_(screenHeight) {
}

void BounceBehavior::update(float dt) {
    auto* body = owner_->getComponent<BodyComponent>();
    if (!body) return;
    
    float x = body->getX();
    float y = body->getY();
    float vx = body->getVelocityX();
    float vy = body->getVelocityY();
    
    // Bounce off walls with tolerance to go slightly off screen
    if (x < -20) {
        body->setX(-20);
        body->setVelocity(-vx, vy);
    }
    if (x + body->getWidth() > screenWidth_ + 20) {
        body->setX(screenWidth_ + 20 - body->getWidth());
        body->setVelocity(-vx, vy);
    }
    
    // Keep in bottom half and bounce off top/bottom boundaries with tolerance
    if (y < screenHeight_ / 2 - 20) {
        body->setY(screenHeight_ / 2 - 20);
        body->setVelocity(vx, -vy);
    }
    if (y + body->getHeight() > screenHeight_ + 20) {
        body->setY(screenHeight_ + 20 - body->getHeight());
        body->setVelocity(vx, -vy);
    }
}

// ProjectileBehavior implementation
ProjectileBehavior::ProjectileBehavior(int screenHeight)
    : screenHeight_(screenHeight) {
}

void ProjectileBehavior::update(float dt) {
    auto* body = owner_->getComponent<BodyComponent>();
    if (!body) return;
    
    if (body->getY() > screenHeight_) {
        offScreen_ = true;
    }
}
