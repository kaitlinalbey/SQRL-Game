#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld(float gravityX, float gravityY) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {gravityX, gravityY};
    worldId_ = b2CreateWorld(&worldDef);
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
