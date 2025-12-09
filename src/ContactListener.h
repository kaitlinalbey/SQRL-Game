#pragma once

#include <box2d/box2d.h>
#include <functional>

class ContactListener {
public:
    using CollisionCallback = std::function<void(void* bodyA, void* bodyB)>;
    
    ContactListener();
    ~ContactListener();
    
    void setCallback(CollisionCallback callback) { callback_ = callback; }
    
    // Box2D contact callbacks
    static bool beginContact(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);
    
private:
    CollisionCallback callback_;
};
