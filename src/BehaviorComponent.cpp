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
    
    // For leaf (no physics), use simple bouncing with random direction changes
    if (!body->hasPhysicsBody()) {
        // Bounce off walls
        if (x < 0) {
            body->setX(0);
            body->setVelocity(-vx, vy);
        }
        if (x + body->getWidth() > screenWidth_) {
            body->setX(screenWidth_ - body->getWidth());
            body->setVelocity(-vx, vy);
        }
        if (y < screenHeight_ / 2) {
            body->setY(screenHeight_ / 2);
            body->setVelocity(vx, -vy);
        }
        if (y + body->getHeight() > screenHeight_) {
            body->setY(screenHeight_ - body->getHeight());
            body->setVelocity(vx, -vy);
        }
        
        // Random direction changes for floating effect (5% chance, gentler)
        if (rand() % 100 < 5) {
            float changeX = ((rand() % 100) - 50) * 0.5f;  // Much smaller changes
            float changeY = ((rand() % 100) - 50) * 0.5f;
            body->setVelocity(vx + changeX, vy + changeY);
        }
        
        return;
    }
    
    // For red bird (has physics), use physics-based bouncing
    if (body->hasPhysicsBody()) {
        // Physics-based bouncing - apply impulses to reverse velocity
        b2BodyId bodyId = body->getPhysicsBodyId();
        b2Vec2 pos = b2Body_GetPosition(bodyId);
        b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
        
        bool applyImpulse = false;
        b2Vec2 newVel = vel;
        
        // Bounce off walls
        if (x < 0) {
            b2Body_SetTransform(bodyId, {5.0f, pos.y}, b2Body_GetRotation(bodyId));
            newVel.x = std::abs(vel.x);
            applyImpulse = true;
        }
        if (x + body->getWidth() > screenWidth_) {
            b2Body_SetTransform(bodyId, {screenWidth_ - body->getWidth() - 5.0f, pos.y}, b2Body_GetRotation(bodyId));
            newVel.x = -std::abs(vel.x);
            applyImpulse = true;
        }
        
        // Bounce in bottom half only
        if (y < screenHeight_ / 2) {
            b2Body_SetTransform(bodyId, {pos.x, screenHeight_ / 2.0f + 5.0f}, b2Body_GetRotation(bodyId));
            newVel.y = std::abs(vel.y);
            applyImpulse = true;
        }
        if (y + body->getHeight() > screenHeight_) {
            b2Body_SetTransform(bodyId, {pos.x, screenHeight_ - body->getHeight() - 5.0f}, b2Body_GetRotation(bodyId));
            newVel.y = -std::abs(vel.y);
            applyImpulse = true;
        }
        
        if (applyImpulse) {
            b2Body_SetLinearVelocity(bodyId, newVel);
        }
        
        // Random direction changes (15% chance per update) with stronger impulses
        if (rand() % 100 < 15) {
            float changeX = ((rand() % 200) - 100) * 3.0f;
            float changeY = ((rand() % 200) - 100) * 3.0f;
            b2Body_ApplyLinearImpulse(bodyId, {changeX, changeY}, pos, true);
        }
        
        // Ensure minimum velocity to prevent stopping
        if (std::abs(vel.x) < 50.0f && std::abs(vel.y) < 50.0f) {
            float boostX = (rand() % 2 == 0 ? 1 : -1) * 100.0f;
            float boostY = (rand() % 2 == 0 ? 1 : -1) * 100.0f;
            b2Body_ApplyLinearImpulse(bodyId, {boostX, boostY}, pos, true);
        }
    } else {
        // Fallback for non-physics bodies
        bool needsSync = false;
        
        if (x < 0) {
            body->setX(0);
            body->setVelocity(-vx, vy);
            needsSync = true;
        }
        if (x + body->getWidth() > screenWidth_) {
            body->setX(screenWidth_ - body->getWidth());
            body->setVelocity(-vx, vy);
            needsSync = true;
        }
        if (y < screenHeight_ / 2) {
            body->setY(screenHeight_ / 2);
            body->setVelocity(vx, -vy);
            needsSync = true;
        }
        if (y + body->getHeight() > screenHeight_) {
            body->setY(screenHeight_ - body->getHeight());
            body->setVelocity(vx, -vy);
            needsSync = true;
        }
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
