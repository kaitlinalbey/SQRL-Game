#pragma once

#include <box2d/box2d.h>
#include <functional>

class PhysicsWorld {
public:
    PhysicsWorld(float gravityX = 0.0f, float gravityY = 9.8f);
    ~PhysicsWorld();

    // Step the physics simulation
    void step(float deltaTime, int subStepCount = 4);

    // Access the Box2D world ID
    b2WorldId getWorldId() const { return worldId_; }

    // World settings
    void setGravity(float x, float y);
    b2Vec2 getGravity() const;
    
    // Collision callback
    using CollisionCallback = std::function<void(void* bodyA, void* bodyB)>;
    void setContactCallback(CollisionCallback callback);

private:
    b2WorldId worldId_;
    CollisionCallback contactCallback_;
    
    static bool contactBeginCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);
};
