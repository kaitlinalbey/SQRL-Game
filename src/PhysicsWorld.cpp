#include "PhysicsWorld.h"
#include "BodyComponent.h"
#include <iostream>

PhysicsWorld::PhysicsWorld(float gravityX, float gravityY) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {gravityX, gravityY};
    worldId_ = b2CreateWorld(&worldDef);
    
    // Register contact callback
    b2World_SetPreSolveCallback(worldId_, contactBeginCallback, this);
}

PhysicsWorld::~PhysicsWorld() {
    if (B2_IS_NON_NULL(worldId_)) {
        b2DestroyWorld(worldId_);
    }
}

void PhysicsWorld::step(float deltaTime, int subStepCount) {
    if (B2_IS_NON_NULL(worldId_)) {
        b2World_Step(worldId_, deltaTime, subStepCount);
    }
}

void PhysicsWorld::setGravity(float x, float y) {
    if (B2_IS_NON_NULL(worldId_)) {
        b2World_SetGravity(worldId_, {x, y});
    }
}

b2Vec2 PhysicsWorld::getGravity() const {
    if (B2_IS_NON_NULL(worldId_)) {
        return b2World_GetGravity(worldId_);
    }
    return {0.0f, 0.0f};
}

void PhysicsWorld::setContactCallback(CollisionCallback callback) {
    contactCallback_ = callback;
}

bool PhysicsWorld::contactBeginCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context) {
    PhysicsWorld* world = static_cast<PhysicsWorld*>(context);
    if (!world || !world->contactCallback_) return true;
    
    // Get bodies from shapes
    b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
    b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);
    
    // Get user data (BodyComponent pointers)
    void* userDataA = b2Body_GetUserData(bodyIdA);
    void* userDataB = b2Body_GetUserData(bodyIdB);
    
    if (userDataA && userDataB) {
        world->contactCallback_(userDataA, userDataB);
    }
    
    return true; // Allow collision
}
