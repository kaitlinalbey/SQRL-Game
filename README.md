# Squirrel Acorn Game - Box2D Physics Integration

A 2D arcade game built with SDL2 and Box2D 3.0, featuring physics-based gameplay where a squirrel shoots acorns at moving targets.

## Project Overview

This project demonstrates the integration of Box2D physics engine with SDL2 for game development. The game features gravity, collisions, bouncing mechanics, and dynamic physics bodies.

## Screenshots

### Gameplay
![Gameplay Screenshot - Main Level](screenshots/gameplay_level1.png)
*Level 1: Shoot acorns at the floating leaf to score points*

![Gameplay Screenshot - Level 2](screenshots/gameplay_level2.png)
*Level 2: Added red bird target with higher point value*

### Physics Demonstrations
![Physics Demo - Spinning Acorns](screenshots/physics_spin.png)
*Acorns automatically spin as they fall (angular velocity demonstration)*

![Physics Demo - Bouncing](screenshots/physics_bounce.png)
*Acorns bounce off targets (linear velocity demonstration)*

## Features

### Gameplay Features
- Two difficulty levels with increasing challenges
- Physics-based projectile mechanics with gravity
- Dynamic target spawning and movement
- Score tracking and win/lose conditions
- Realistic bouncing and collision responses

### Box2D Physics Integration

#### 1. **Box2D with SDL Integration**
- Custom `PhysicsWorld` wrapper manages Box2D `b2WorldId`
- `BodyComponent` extends game objects with physics bodies (`b2BodyId`)
- Physics simulation synchronized with SDL rendering at 60 FPS
- Automatic cleanup of physics bodies when game objects are destroyed

**Key Files:**
- `src/PhysicsWorld.h/cpp` - Box2D world management
- `src/BodyComponent.h/cpp` - Physics body integration with game objects
- `src/Game.cpp` - Physics step integration in game loop

#### 2. **userData Field Usage**
- Each `b2Body` stores a pointer to its `BodyComponent` via `userData`
- Enables retrieval of game objects during collision callbacks
- Used in contact listener for collision response logic

**Implementation:** `src/BodyComponent.cpp` line 46, `src/ContactListener.cpp`

#### 3. **Dynamic Forces and Velocities**

**Linear Velocity:**
- Acorns bounce off targets by reversing and dampening vertical velocity
- Manual velocity changes applied through `b2Body_SetLinearVelocity()`

**Angular Velocity:**
- Acorns automatically spin when shot using `b2Body_SetAngularVelocity()`
- Rotation synchronized from physics to visual rendering

**Implementation:** `src/Game.cpp` collision handlers, `src/BodyComponent.cpp` rotation sync

#### 4. **Physics Queries**

**AABB Querying:**
- Manual Axis-Aligned Bounding Box collision detection
- Used for detecting acorn-leaf and acorn-red bird collisions
- Implemented because targets use simple velocity (no physics bodies)

**Contact Listening:**
- `ContactListener` class handles Box2D collision callbacks
- `b2World_SetPreSolveCallback` registers collision handler
- Retrieves game objects via `userData` for collision response

**Implementation:** `src/ContactListener.h/cpp`, `src/Game.cpp` collision handling

#### 5. **Adding and Removing Bodies at Runtime**

**Adding Bodies:**
- Press W/Up Arrow to shoot acorns - creates new physics bodies dynamically
- `spawnAcorn()` function creates `GameObject` with dynamic `b2Body`
- Bodies initialized with gravity, restitution, and velocity properties

**Removing Bodies:**
- Physics bodies automatically destroyed when game objects deactivate
- Acorns removed when going off-screen or hitting targets
- `BodyComponent::destroyPhysicsBody()` handles cleanup

**Implementation:** `src/Game.cpp` spawnAcorn(), acorn cleanup loop

## Controls

### Gameplay
- **Arrow Keys / A, D** - Move squirrel left/right
- **W / Up Arrow** - Shoot acorn
- **Space / Enter** - Start game from title screen

## Technical Details

### Architecture
- **Component-Based Design:** GameObjects composed of Components (Body, Sprite, Controller, Behavior)
- **MVC Pattern:** Separate Graphics, Input, and View abstraction layers
- **Factory Pattern:** ObjectFactory for dynamic object creation
- **RAII:** Automatic resource cleanup using smart pointers and destructors

### Physics Configuration
- **Gravity:** 400 units downward (Y-axis positive)
- **Frame Rate:** 60 FPS with delta time
- **Acorn Physics:** Dynamic bodies with 0.15 restitution (low bounce)
- **Squirrel Physics:** Kinematic body (player-controlled, not affected by gravity)
- **Hybrid System:** Acorns use Box2D physics, targets use simple velocity movement

### Box2D 3.0 API
- Uses modern C-style API with opaque handles (`b2WorldId`, `b2BodyId`)
- World creation: `b2CreateWorld(&b2WorldDef)`
- Body creation: `b2CreateBody()` with `b2BodyDef` and `b2ShapeDef`
- Stepping simulation: `b2World_Step()` called each frame

### Dependencies
- **SDL2** - Graphics, window management, input
- **SDL_image** - Texture loading
- **SDL_ttf** - Text rendering
- **Box2D 3.0** - Physics engine
- **tinyxml2** - Configuration file parsing

### Build System
- **CMake 3.21+** with vcpkg package manager
- **Compiler:** MinGW GCC (C++17 standard)
- **Platform:** Windows (can be adapted for cross-platform)

## Building and Running

### Prerequisites
```powershell
# Ensure vcpkg and CMake are installed
# Dependencies managed through vcpkg.json
```

### Build Commands
```powershell
# Configure (first time only)
cmake --preset win-mingw-debug

# Build
cmake --build build/win-mingw-debug

# Run
.\build\win-mingw-debug\demo.exe
```

## Code Structure

### Core Game Files
- `src/main.cpp` - Entry point
- `src/Game.h/cpp` - Main game loop, physics integration, collision handling
- `src/GameObject.h/cpp` - Game object container

### Component System
- `src/Component.h` - Base component interface
- `src/BodyComponent.h/cpp` - Position, velocity, physics body management
- `src/SpriteComponent.h/cpp` - Texture rendering with rotation
- `src/ControllerComponent.h/cpp` - Player input handling
- `src/BehaviorComponent.h/cpp` - AI behaviors (bouncing, projectile)

### Physics Integration
- `src/PhysicsWorld.h/cpp` - Box2D world wrapper
- `src/ContactListener.h/cpp` - Collision callback handler

### Engine Abstraction
- `src/Graphics.h/cpp` - SDL rendering wrapper
- `src/Input.h/cpp` - SDL input wrapper
- `src/View.h/cpp` - Camera/viewport transforms

### Factory and Utilities
- `src/ObjectFactory.h/cpp` - Dynamic object creation
- `src/ImageDevice.h/cpp` - Texture management
- `src/InputDevice.h/cpp` - Input device abstraction

## Box2D Integration Highlights

### Where Box2D and SDL Integrate

**1. World Creation (src/PhysicsWorld.cpp)**
```cpp
// Create Box2D world with gravity
b2WorldDef worldDef = b2DefaultWorldDef();
worldDef.gravity = {0.0f, 400.0f}; // Downward gravity
worldId_ = b2CreateWorld(&worldDef);
```

**2. Game Loop Integration (src/Game.cpp)**
```cpp
// Physics step called each frame before updates
physicsWorld_.step(dt); // Advance Box2D simulation

// Game objects update
squirrel_->update(dt);
for (auto& acorn : acorns_) {
    acorn->update(dt); // Syncs position from physics
}

// Render objects at physics-synchronized positions
```

**3. Physics Body Creation (src/BodyComponent.cpp)**
```cpp
// Create physics body for game object
b2BodyDef bodyDef = b2DefaultBodyDef();
bodyDef.type = b2_dynamicBody; // or b2_kinematicBody
bodyDef.position = {x_, y_};
physicsBodyId_ = b2CreateBody(world->getWorldId(), &bodyDef);

// Create collision shape
b2Polygon boxShape = b2MakeBox(width_ / 2.0f, height_ / 2.0f);
b2ShapeDef shapeDef = b2DefaultShapeDef();
shapeDef.material.restitution = restitution; // Bounciness
b2CreatePolygonShape(physicsBodyId_, &shapeDef, &boxShape);

// CRITICAL: Store game object reference for collision callbacks
b2Body_SetUserData(physicsBodyId_, this);
```

**4. Position Sync (src/BodyComponent.cpp)**
```cpp
// Copy physics position to visual position each frame
void BodyComponent::syncFromPhysics() {
    b2Vec2 position = b2Body_GetPosition(physicsBodyId_);
    x_ = position.x;
    y_ = position.y;
    
    // Also sync rotation for spinning objects
    b2Rot rot = b2Body_GetRotation(physicsBodyId_);
    rotation_ = b2Rot_GetAngle(rot);
}
```

**5. Rendering with Rotation (src/SpriteComponent.cpp)**
```cpp
// SDL renders sprite at physics-synchronized position and rotation
double angleDegrees = body->getRotation() * (180.0 / M_PI);
SDL_RenderCopyEx(renderer_, texture_, nullptr, &destRect, 
                 angleDegrees, nullptr, SDL_FLIP_NONE);
```

**6. Collision Response (src/Game.cpp)**
```cpp
// Manual AABB collision check (for non-physics targets)
if (ax < lx + lw && ax + aw > lx && 
    ay < ly + lh && ay + ah > ly) {
    // Apply bounce by reversing velocity
    b2Vec2 vel = b2Body_GetLinearVelocity(acornBodyId);
    b2Body_SetLinearVelocity(acornBodyId, 
        {vel.x * 0.8f, -vel.y * 0.6f}); // Dampen and reverse
}
```

**7. Contact Listener (src/ContactListener.cpp)**
```cpp
// Box2D callback when physics bodies collide
bool ContactListener::beginContact(b2ShapeId shapeIdA, b2ShapeId shapeIdB, 
                                   b2Manifold* manifold, void* context) {
    // Get bodies from shapes
    b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
    b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);
    
    // Retrieve game objects via userData
    void* userDataA = b2Body_GetUserData(bodyIdA);
    void* userDataB = b2Body_GetUserData(bodyIdB);
    
    // Call game-specific collision handler
    listener->callback_(userDataA, userDataB);
    return true;
}
```

## Game Design Decisions

### Hybrid Physics System
- **Acorns:** Full Box2D physics (gravity, bounce, spin)
- **Targets (Leaf, Red Bird):** Simple velocity movement
- **Reason:** Manual AABB collision detection easier without physics constraints
- **Benefit:** Gameplay control + realistic projectile physics

### Memory Management
- Smart pointers (`std::unique_ptr`) for automatic cleanup
- Physics bodies destroyed in `BodyComponent` destructor
- No manual memory management required

### Performance
- Physics step: 4 sub-steps per frame for stability
- Object pooling via vector with active flags
- Efficient removal using `std::remove_if`

## Learning Outcomes

This project demonstrates:
- Integration of physics engine with game engine
- Component-based game architecture
- Real-time physics simulation and rendering
- Collision detection and response
- Resource management with RAII
- Factory pattern for object creation
- Separation of concerns (MVC pattern)

## Future Enhancements

Potential improvements:
- Add particle effects for collisions
- Implement sound effects
- Add more levels with varying difficulties
- Power-ups and special acorn types
- High score persistence
- Physics-based obstacles that affect acorn trajectory

## Author

Katlin Albey

## License

Educational project for game programming coursework.

