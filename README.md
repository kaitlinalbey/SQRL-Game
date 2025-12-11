# Project 02 - Component-Based Game Architecture

A 2D arcade game built with SDL2 demonstrating a modular component-based architecture. Players control a squirrel shooting acorns at moving targets using interchangeable, reusable components.

## Project Overview

This project implements a component-based game engine architecture where game objects are composed of modular components rather than inheritance hierarchies. The system uses a factory pattern to create objects from XML definitions, allowing for flexible object creation and easy extensibility.

## Screenshots

### Gameplay
![Gameplay Screenshot - Level 1](screenshots/gameplay_level1.png)
*Level 1: Component-based player control and behavior interactions*

![Gameplay Screenshot - Level 2](screenshots/gameplay_level2.png)
*Level 2: Multiple objects with different component combinations*

### Component Architecture
![Component Diagram](screenshots/component_architecture.png)
*Modular component system allowing flexible object composition*

![XML Factory](screenshots/xml_factory.png)
*Objects defined in XML and instantiated through factory pattern*

## Component-Based Architecture

### Core Design Principles

**Composition over Inheritance:** Instead of creating multiple GameObject subclasses, objects are composed of interchangeable components. This allows for flexible object creation and easy addition of new behaviors without modifying existing code.

**Modular Components:** Each component handles a specific aspect of game object functionality (rendering, physics, input, behavior), making the system maintainable and extensible.

## Assignment Requirements Implemented

### 1. GameObject Class (Container Pattern)

The `GameObject` class acts as a component container with no subclasses.

**Key Features:**
- Stores components in `vector<unique_ptr<Component>>`
- Template method `getComponent<T>()` retrieves components by type
- `addComponent()` adds new components dynamically
- `update()` and `render()` methods iterate through all components

**Implementation:** `src/GameObject.h/cpp`

```cpp
class GameObject {
    vector<unique_ptr<Component>> components_;
    
    template<typename T>
    T* getComponent();  // Retrieve component by type
    
    void addComponent(unique_ptr<Component> comp);
    void update(float dt);  // Calls update on all components
    void render();          // Calls render on all components
};
```

### 2. Component Hierarchy (Abstract Base)

All components derive from the abstract `Component` base class.

**Required Methods:**
- `virtual void update(float dt) = 0` - Pure virtual update
- `virtual void render() = 0` - Pure virtual render
- `GameObject* getOwner()` - Returns reference to parent GameObject

**Implementation:** `src/Component.h`

```cpp
class Component {
public:
    virtual ~Component() = default;
    virtual void update(float dt) {}
    virtual void render() {}
    virtual string getType() const = 0;
    
    GameObject* getOwner() const { return owner_; }  // Parent access
    void setOwner(GameObject* owner) { owner_ = owner; }
protected:
    GameObject* owner_ = nullptr;
};
```

### 3. Required Components

#### SpriteComponent - Graphical Rendering
Manages texture rendering and visual representation.

**Functionality:**
- Loads and renders SDL textures
- Integrates with SDL2 rendering pipeline
- Transforms world coordinates to screen space
- Supports rotation for spinning objects

**Implementation:** `src/SpriteComponent.h/cpp`

#### BodyComponent - Physics Management
Manages position, velocity, and Box2D physics integration.

**Functionality:**
- Position (x, y) and velocity (vx, vy) tracking
- Angle/rotation support
- Box2D physics body integration
- Position synchronization between physics and visuals

**Implementation:** `src/BodyComponent.h/cpp`

```cpp
class BodyComponent : public Component {
    float x_, y_;           // Position
    float velocityX_, velocityY_;  // Velocity
    float rotation_;        // Angle
    b2BodyId physicsBodyId_;  // Box2D integration
    
    void syncFromPhysics();  // Update from physics
    void syncToPhysics();    // Update physics from component
};
```

#### ControllerComponent - Player Input
Handles player input for movement control.

**Functionality:**
- Keyboard input processing (arrow keys, WASD)
- Movement speed configuration
- Integrates with Input class (not raw SDL events)
- Updates BodyComponent position based on input

**Implementation:** `src/ControllerComponent.h/cpp`

```cpp
class ControllerComponent : public Component {
    float speed_;
    
    void moveLeft(float dt);
    void moveRight(float dt);
    void update(float dt) override;  // Reads from Input class
};
```

#### BehaviorComponent - AI Behaviors
Custom behavior components for different object types.

**Implemented Behaviors:**
- **BounceBehavior** - Objects bounce off screen boundaries with random direction changes
- **ProjectileBehavior** - Upward projectile motion for acorns

**Implementation:** `src/BehaviorComponent.h/cpp`

```cpp
class BounceBehavior : public BehaviorComponent {
    void update(float dt) override;  // Boundary collision and direction changes
};

class ProjectileBehavior : public BehaviorComponent {
    void update(float dt) override;  // Upward velocity, off-screen detection
};
```

### 4. Component Factory (XML-Based Object Creation)

The factory pattern allows object creation from XML definitions, enabling data-driven design.

**Factory Features:**
- Reads object definitions from XML files
- Uses `map<string, function>` to map type names to factory functions
- Dynamically creates GameObjects with specified components
- Initializes components after creation

**Implementation:** `src/ObjectFactory.h/cpp`

```cpp
class ObjectFactory {
    unordered_map<string, function<unique_ptr<GameObject>(const ObjectParams&)>> factories_;
    
    void registerType(const string& type, function<...> factory);
    unique_ptr<GameObject> create(const string& type, const ObjectParams& params);
};
```

### XML Object Definitions

Objects are defined in `assets/objects.xml` with their component composition:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<ObjectDefinitions>
    <!-- Player object with controller for input -->
    <Object type="Squirrel">
        <Component type="BodyComponent" x="400" y="50" width="80" height="80"/>
        <Component type="SpriteComponent" texture="SQRL"/>
        <Component type="ControllerComponent" speed="300"/>
    </Object>
    
    <!-- AI-controlled target with bounce behavior -->
    <Object type="Leaf">
        <Component type="BodyComponent" x="400" y="500" width="90" height="90"/>
        <Component type="SpriteComponent" texture="leaf"/>
        <Component type="BounceBehavior"/>
    </Object>
    
    <!-- Projectile with upward motion -->
    <Object type="Acorn">
        <Component type="BodyComponent" width="30" height="30"/>
        <Component type="SpriteComponent" texture="acorn"/>
        <Component type="ProjectileBehavior"/>
    </Object>
</ObjectDefinitions>
```

**Key Advantages:**
- New object types can be created without code changes
- Component combinations defined in data files
- Easy balancing and tweaking through XML editing
- Clear separation between engine code and game content

### 5. Input Class Integration

Components interact with the `Input` class rather than handling SDL events directly.

**Implementation:** `src/Input.h/cpp`

```cpp
class Input {
    bool isKeyDown(Key key);
    bool isKeyPressed(Key key);
    bool processEvents();  // Updates input state each frame
};
```

**Usage in ControllerComponent:**
```cpp
void ControllerComponent::update(float dt) {
    if (input_.isKeyDown(Key::LEFT)) { moveLeft(dt); }
    if (input_.isKeyDown(Key::RIGHT)) { moveRight(dt); }
    // No direct SDL event handling
}
```

## Playable Demo Features

The game demonstrates all required component interactions:

### Player Movement
- **Arrow Keys / A, D** - Move squirrel left/right (ControllerComponent)
- **W / Up Arrow** - Shoot acorns (spawns GameObject with ProjectileBehavior)
- Diagonal movement possible when moving and shooting simultaneously

### Component Interactions
- **ControllerComponent** → **BodyComponent**: Input updates position
- **BodyComponent** → **SpriteComponent**: Position determines rendering location
- **BehaviorComponent** → **BodyComponent**: AI behaviors modify velocity/position
- **ProjectileBehavior** checks for off-screen (component communication)

### Dynamic Object Creation
- Pressing W/Up spawns new acorn GameObjects at runtime
- Each acorn gets BodyComponent, SpriteComponent, and ProjectileBehavior
- Objects removed when inactive (memory management through smart pointers)

### AI Behaviors
- Leaf bounces with BounceBehavior (screen boundary detection)
- Random direction changes create unpredictable movement
- No hard-coded behaviors - all component-driven

## Code Structure

### Component System Files
- `src/Component.h` - Abstract base class with virtual update/render
- `src/GameObject.h/cpp` - Component container (no subclasses)
- `src/BodyComponent.h/cpp` - Position, velocity, angle management
- `src/SpriteComponent.h/cpp` - Texture rendering
- `src/ControllerComponent.h/cpp` - Player input handling
- `src/BehaviorComponent.h/cpp` - AI behaviors (BounceBehavior, ProjectileBehavior)

### Factory System Files
- `src/ObjectFactory.h/cpp` - XML-based object creation with component factory
- `assets/objects.xml` - Object definitions with component composition

### Engine Abstraction Files
- `src/Input.h/cpp` - Input wrapper (used by components, not raw SDL)
- `src/Graphics.h/cpp` - SDL rendering abstraction
- `src/View.h/cpp` - Camera/viewport transforms
- `src/Game.h/cpp` - Main game loop

### Additional Features
- `src/PhysicsWorld.h/cpp` - Box2D physics integration (optional enhancement)
- `src/ContactListener.h/cpp` - Collision callback handling

## Technical Implementation

### Component Architecture Benefits
- **Modularity**: Components can be added/removed without affecting others
- **Reusability**: Same components used across different object types
- **Extensibility**: New behaviors added as new component classes
- **Maintainability**: Each component has single responsibility
- **Data-Driven**: Object types defined in XML, not hardcoded

### Design Patterns Used
- **Component Pattern**: Composition over inheritance for game objects
- **Factory Pattern**: XML-driven object instantiation
- **Template Method**: GameObject iterates components calling virtual methods
- **RAII**: Smart pointers ensure automatic cleanup

### Dependencies
- **SDL2** - Graphics, window management
- **SDL_image** - Texture loading
- **SDL_ttf** - Text rendering
- **Box2D 3.0** - Physics engine (optional enhancement)
- **tinyxml2** - XML parsing for factory

### Build System
- **CMake 3.21+** with vcpkg package manager
- **Compiler:** MinGW GCC (C++17 standard)
- **Platform:** Windows (cross-platform compatible)

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

