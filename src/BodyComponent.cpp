#include "BodyComponent.h"
#include "PhysicsWorld.h"

BodyComponent::BodyComponent(float x, float y, float w, float h)
    : x_(x), y_(y), width_(w), height_(h) {
}

void BodyComponent::update(float dt) {
    if (B2_IS_NON_NULL(physicsBodyId_)) {
        // If we have a physics body, sync from physics
        syncFromPhysics();
    } else {
        // Otherwise use simple velocity-based movement
        x_ += velocityX_ * dt;
        y_ += velocityY_ * dt;
    }
}

void BodyComponent::createPhysicsBody(PhysicsWorld* world, b2BodyType type, float restitution, float gravityScale, float linearDamping) {
    if (!world || B2_IS_NON_NULL(physicsBodyId_)) return;
    
    physicsWorld_ = world;
    
    // Create body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = type;
    bodyDef.position = {x_, y_};
    bodyDef.rotation = b2Rot_identity;
    bodyDef.gravityScale = gravityScale;
    bodyDef.linearDamping = linearDamping;
    
    // Create the body in the physics world
    physicsBodyId_ = b2CreateBody(world->getWorldId(), &bodyDef);
    
    // Create a box shape
    b2Polygon boxShape = b2MakeBox(width_ / 2.0f, height_ / 2.0f);
    
    // Create shape definition with physics properties
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = restitution;  // Bounciness (configurable)
    
    // Create the shape on the body
    b2CreatePolygonShape(physicsBodyId_, &shapeDef, &boxShape);
    
    // Store pointer to this component in user data
    b2Body_SetUserData(physicsBodyId_, this);
}

void BodyComponent::destroyPhysicsBody() {
    if (B2_IS_NON_NULL(physicsBodyId_)) {
        b2DestroyBody(physicsBodyId_);
        physicsBodyId_ = b2_nullBodyId;
        physicsWorld_ = nullptr;
    }
}

void BodyComponent::syncFromPhysics() {
    if (B2_IS_NON_NULL(physicsBodyId_)) {
        b2Vec2 position = b2Body_GetPosition(physicsBodyId_);
        x_ = position.x;
        y_ = position.y;
        
        b2Vec2 velocity = b2Body_GetLinearVelocity(physicsBodyId_);
        velocityX_ = velocity.x;
        velocityY_ = velocity.y;
    }
}

void BodyComponent::syncToPhysics() {
    if (B2_IS_NON_NULL(physicsBodyId_)) {
        b2Body_SetTransform(physicsBodyId_, {x_, y_}, b2Body_GetRotation(physicsBodyId_));
        b2Body_SetLinearVelocity(physicsBodyId_, {velocityX_, velocityY_});
    }
}
