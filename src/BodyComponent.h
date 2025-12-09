#pragma once
#include "Component.h"
#include <box2d/box2d.h>

class PhysicsWorld;

class BodyComponent : public Component {
public:
    BodyComponent(float x = 0, float y = 0, float w = 0, float h = 0);
    
    std::string getType() const override { return "BodyComponent"; }
    
    float getX() const { return x_; }
    float getY() const { return y_; }
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    
    void setX(float x) { x_ = x; }
    void setY(float y) { y_ = y; }
    void setPosition(float x, float y) { x_ = x; y_ = y; }
    void setSize(float w, float h) { width_ = w; height_ = h; }
    
    float getVelocityX() const { return velocityX_; }
    float getVelocityY() const { return velocityY_; }
    void setVelocity(float vx, float vy) { velocityX_ = vx; velocityY_ = vy; }
    
    float getRotation() const { return rotation_; }
    void setRotation(float rotation) { rotation_ = rotation; }
    
    void update(float dt) override;

    // Box2D physics integration
    void createPhysicsBody(PhysicsWorld* world, b2BodyType type = b2_dynamicBody, float restitution = 0.5f, float gravityScale = 1.0f, float linearDamping = 0.0f);
    void destroyPhysicsBody();
    void syncFromPhysics();  // Copy physics position to visual position
    void syncToPhysics();    // Copy visual position to physics (for kinematic control)
    b2BodyId getPhysicsBodyId() const { return physicsBodyId_; }
    bool hasPhysicsBody() const { return B2_IS_NON_NULL(physicsBodyId_); }

private:
    float x_, y_;
    float width_, height_;
    float velocityX_ = 0;
    float velocityY_ = 0;
    float rotation_ = 0.0f;  // Rotation in radians
    b2BodyId physicsBodyId_ = b2_nullBodyId;
    PhysicsWorld* physicsWorld_ = nullptr;
};
