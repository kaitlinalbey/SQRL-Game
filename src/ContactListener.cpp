#include "ContactListener.h"
#include "BodyComponent.h"
#include <iostream>

ContactListener::ContactListener() {
}

ContactListener::~ContactListener() {
}

bool ContactListener::beginContact(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context) {
    ContactListener* listener = static_cast<ContactListener*>(context);
    if (!listener) return true;
    
    // Get the bodies from the shapes
    b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
    b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);
    
    // Get user data (BodyComponent pointers)
    void* userDataA = b2Body_GetUserData(bodyIdA);
    void* userDataB = b2Body_GetUserData(bodyIdB);
    
    if (userDataA && userDataB && listener->callback_) {
        listener->callback_(userDataA, userDataB);
    }
    
    return true; // Allow collision to proceed
}
