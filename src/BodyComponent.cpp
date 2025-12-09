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

// BOX2D INTEGRATION: Create a physics body for this game object
// Links the visual game object with Box2D physics simulation
void BodyComponent::createPhysicsBody(PhysicsWorld* world, b2BodyType type, float restitution, float gravityScale, float linearDamping) {
    // Don't create if already has physics body or no world provided
    if (!world || B2_IS_NON_NULL(physicsBodyId_)) return;
    
    physicsWorld_ = world;
    
    // Define the physics body properties
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = type; // b2_dynamicBody (affected by forces), b2_kinematicBody (manually controlled), or b2_staticBody
    bodyDef.position = {x_, y_}; // Initial position in world space
    bodyDef.rotation = b2Rot_identity; // Start with no rotation
    bodyDef.gravityScale = gravityScale; // Multiplier for gravity (0 = no gravity, 1 = normal, >1 = heavier)
    bodyDef.linearDamping = linearDamping; // Air resistance (0 = no damping, higher = more resistance)
    
    // Create the body in the Box2D world
    physicsBodyId_ = b2CreateBody(world->getWorldId(), &bodyDef);
    
    // Create a box-shaped collision shape (use object's width and height)
    // Box2D uses half-widths, so divide by 2
    b2Polygon boxShape = b2MakeBox(width_ / 2.0f, height_ / 2.0f);
    
    // Define the shape's material properties
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f; // Mass per unit area
    shapeDef.material.friction = 0.3f; // Surface friction (0 = ice, 1 = rubber)
    shapeDef.material.restitution = restitution; // Bounciness (0 = no bounce, 1 = perfect bounce)
    
    // Attach the shape to the body
    b2CreatePolygonShape(physicsBodyId_, &shapeDef, &boxShape);
    
    // CRITICAL: Store pointer to this component in Box2D body's userData
    // This allows us to retrieve the game object during collision callbacks
    b2Body_SetUserData(physicsBodyId_, this);
}

void BodyComponent::destroyPhysicsBody() {
    if (B2_IS_NON_NULL(physicsBodyId_)) {
        b2DestroyBody(physicsBodyId_);
        physicsBodyId_ = b2_nullBodyId;
        physicsWorld_ = nullptr;
    }
}

// BOX2D-SDL SYNC: Copy physics state to visual representation
// Called each frame to keep the rendered sprite in sync with physics simulation
void BodyComponent::syncFromPhysics() {
    if (B2_IS_NON_NULL(physicsBodyId_)) {
        // Get position from Box2D physics body
        b2Vec2 position = b2Body_GetPosition(physicsBodyId_);
        x_ = position.x;
        y_ = position.y;
        
        // Get velocity from physics body (for display or game logic)
        b2Vec2 velocity = b2Body_GetLinearVelocity(physicsBodyId_);
        velocityX_ = velocity.x;
        velocityY_ = velocity.y;
        
        // Get rotation angle for visual rendering (makes objects spin)
        b2Rot rot = b2Body_GetRotation(physicsBodyId_);
        rotation_ = b2Rot_GetAngle(rot); // Convert Box2D rotation struct to angle in radians
    }
}

void BodyComponent::syncToPhysics() {
    if (B2_IS_NON_NULL(physicsBodyId_)) {
        b2Body_SetTransform(physicsBodyId_, {x_, y_}, b2Body_GetRotation(physicsBodyId_));
        b2Body_SetLinearVelocity(physicsBodyId_, {velocityX_, velocityY_});
    }
}
