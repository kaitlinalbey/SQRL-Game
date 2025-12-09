#include "ContactListener.h"
#include "BodyComponent.h"
#include <iostream>

ContactListener::ContactListener() {
}

ContactListener::~ContactListener() {
}

// BOX2D REQUIREMENT: Contact Listening - collision callback function
// This static function is called by Box2D whenever two physics bodies begin touching
bool ContactListener::beginContact(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context) {
    ContactListener* listener = static_cast<ContactListener*>(context);
    if (!listener) return true;
    
    // Get the physics bodies from the collision shapes
    b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
    b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);
    
    // BOX2D REQUIREMENT: userData retrieval for game object identification
    // Get BodyComponent pointers that were stored in b2Body_SetUserData()
    void* userDataA = b2Body_GetUserData(bodyIdA);
    void* userDataB = b2Body_GetUserData(bodyIdB);
    
    // Call the registered callback with the game object components
    if (userDataA && userDataB && listener->callback_) {
        listener->callback_(userDataA, userDataB);
    }
    
    return true; // Allow collision to proceed (return false to cancel collision)
}
