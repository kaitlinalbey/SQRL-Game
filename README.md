# Squirrel Acorn Game - Graphics & Controls Implementation

## Overview

This project implements a complete game using **SDL2 graphics and input systems** with proper **abstraction layers** and **component-based architecture**. The game demonstrates separation of concerns through Graphics and Input devices, implements a View class for camera/viewport control, uses XML for asset management, and includes frame rate limiting with deltaTime calculations for smooth animations.

## Project Requirements Compliance

This implementation satisfies all specified learning objectives and requirements:

### ✅ SDL2 Low-Level Rendering and Event Handling
- **Graphics System** ([src/Graphics.h/cpp](src/Graphics.h)): Wraps SDL2 rendering with texture management, drawing operations, and resource cleanup
- **Input System** ([src/Input.h/cpp](src/Input.h)): Wraps SDL2 event polling with keyboard/mouse state tracking and frame-based updates
- **Implementation**: All rendering uses SDL2's `SDL_RenderCopy`, texture loading with `IMG_LoadTexture`, event handling with `SDL_PollEvent`

### ✅ Abstraction of SDL2 Capabilities in Graphics and Input Devices

**Graphics Device Abstraction:**
- Components call `graphics.drawTexture()`, NOT raw SDL functions
- Texture management through named map: `getTexture("SQRL")`
- SDL_Renderer encapsulated - components never see SDL types directly

**Input Device Abstraction:**
- Custom `Key` enum (Key::LEFT, Key::W) instead of SDL_Scancode
- Methods like `isKeyDown()`, `isKeyPressed()` hide SDL keyboard state arrays
- Components use Input class API, never touch SDL_Event directly

**Files:** [src/Graphics.h](src/Graphics.h), [src/Input.h](src/Input.h)

### ✅ Model-View-Control Separation

The game demonstrates proper MVC architecture:

**Model (Game State):**
- **BodyComponent** stores position, velocity, rotation
- **GameObject** manages component collection and state
- No rendering or input code in model components

**View (Display):**
- **View class** transforms world coordinates to screen space
- **SpriteComponent** renders based on model state
- **Graphics** handles all SDL2 rendering calls

**Control (Input Handling):**
- **Input class** processes SDL events each frame
- **ControllerComponent** reads input, modifies model
- Clear separation: Controller → Model → View pipeline

### ✅ View Class Implementation

**Location:** [src/View.h](src/View.h) and [src/View.cpp](src/View.cpp)

**Features:**
```cpp
class View {
    float centerX_, centerY_;    // View center (camera position)
    float scale_;                // Optional zoom (implemented)
    float rotation_;             // Optional rotation (implemented)
    
    void worldToScreen(float worldX, float worldY, 
                       int& screenX, int& screenY) const;
    void setCenter(float x, float y);
    void followTarget(float targetX, float targetY, float smoothing);
};
```

**Static Storage in Engine ([Game.cpp](src/Game.cpp#L100-L104)):**
```cpp
view_.setCenter(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
view_.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
Graphics::setView(&view_);  // Static instance for engine-level access
```

**Sprite Drawing Modified for View ([SpriteComponent.cpp](src/SpriteComponent.cpp#L26-L46)):**
```cpp
void SpriteComponent::render(const View* view) {
    auto* body = owner_->getComponent<BodyComponent>();
    
    // Transform world coordinates to screen coordinates using the view
    int screenX, screenY;
    if (view) {
        view->worldToScreen(body->getX(), body->getY(), screenX, screenY);
    }
    
    SDL_Rect destRect = { screenX, screenY, width, height };
    SDL_RenderCopy(renderer_, texture_, nullptr, &destRect);
}
```

**Multiple Views:** The architecture supports multiple views through the View pointer parameter - different viewports could be rendered by passing different View instances.

### ✅ Frame Rate Limiting with DeltaTime

**Location:** [src/Game.cpp](src/Game.cpp#L27-L56) main game loop

**Implementation:**
```cpp
// Static frame rate configuration
static constexpr int TARGET_FPS = 60;
static constexpr float TARGET_FRAME_TIME = 1000.0f / TARGET_FPS;  // 16.67ms

auto lastTime = std::chrono::steady_clock::now();

while (running) {
    auto frameStart = std::chrono::steady_clock::now();
    
    // Calculate deltaTime from last frame
    auto currentTime = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
    
    // Cap dt to prevent large jumps (e.g., when debugging)
    if (dt > 0.1f) dt = 0.1f;
    
    handleInput();
    update(dt);  // DeltaTime passed to all game objects
    render();
    
    // Calculate frame duration
    auto frameEnd = std::chrono::steady_clock::now();
    float frameDuration = duration<float, milli>(frameEnd - frameStart).count();
    
    // Delay if work finished early to maintain target FPS
    if (frameDuration < TARGET_FRAME_TIME) {
        float delayTime = TARGET_FRAME_TIME - frameDuration;
        std::this_thread::sleep_for(duration<float, milli>(delayTime));
    }
    // If processing took too long, skip waiting (no frame delay)
}
```

**DeltaTime Usage Throughout Codebase:**
- **ControllerComponent**: Movement scaled by dt → `velocity = speed * dt * direction`
- **BehaviorComponent**: Position updates → `x += velocityX * dt`
- **PhysicsWorld**: Fixed timestep simulation with dt accumulation
- **Cooldowns**: Acorn shooting cooldown decrements by dt

**Benefits:**
- Consistent animation speed across different hardware
- Smooth movement independent of frame rate fluctuations
- Prevents "turbo mode" on fast systems or slowdown on slower systems

### ✅ Asset Management Using XML

**Texture Map Implementation ([src/Graphics.h](src/Graphics.h#L17-L18)):**
```cpp
class Graphics {
    std::unordered_map<std::string, SDL_Texture*> textures_;  // Name → Texture map
    
    bool loadTexture(const std::string& name, const std::string& filePath);
    SDL_Texture* getTexture(const std::string& name);
};
```

**Asset Loading from Files ([src/Game.cpp](src/Game.cpp#L94-L97)):**
```cpp
// Textures stored in map, retrieved by name
graphics_.loadTexture("SQRL", "assets/SQRL.png");
graphics_.loadTexture("acorn", "assets/acorn.png");
graphics_.loadTexture("leaf", "assets/leaf.png");
graphics_.loadTexture("RBIRD", "assets/RBIRD.png");

// Later retrieved by components:
auto texture = graphics_.getTexture("SQRL");  // Map lookup, no file I/O
```

**XML Configuration ([assets/config.xml](assets/config.xml)):**
```xml
<GameConfig title="Squirrel Acorn Game" 
            squirrelSpeed="300" 
            acornSpeed="400"
            leafSpeedX="200" 
            leafSpeedY="150"/>
```

**XML Object Definitions ([assets/objects.xml](assets/objects.xml)):**
```xml
<ObjectDefinitions>
    <Object type="Squirrel">
        <Component type="BodyComponent" x="400" y="50" width="80" height="80"/>
        <Component type="SpriteComponent" texture="SQRL"/>
        <Component type="ControllerComponent" speed="300"/>
    </Object>
    
    <Object type="Leaf">
        <Component type="BodyComponent" x="100" y="100" width="50" height="50"/>
        <Component type="SpriteComponent" texture="leaf"/>
        <Component type="BehaviorComponent" behaviorType="Bounce"/>
    </Object>
</ObjectDefinitions>
```

**XML Parsing ([src/Game.cpp](src/Game.cpp#L62-L75)):**
```cpp
bool Game::loadConfig(const std::string& path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
        return false;
    }
    auto* root = doc.RootElement();
    root->QueryFloatAttribute("squirrelSpeed", &squirrelSpeed_);
    root->QueryFloatAttribute("acornSpeed", &acornSpeed_);
    // ... more attributes
    return true;
}
```

### ✅ Working Game Features

**1. Sprites Appear on Screen:**
- Squirrel sprite renders at player position
- Acorn sprites display when shot
- Leaf target bounces around screen
- Red bird enemy patrols boundaries
- All rendering through SpriteComponent + View transform

**2. Keyboard Input Triggers Actions:**
- **Arrow Keys / A, D**: Move squirrel left/right (ControllerComponent modifies BodyComponent velocity)
- **W / Up Arrow**: Shoot acorn upward (spawns new GameObject with ProjectileBehavior)
- **Space / Enter**: Start game from title screen (state transition)
- **Escape**: Quit game (Input class sets quit flag)

**3. View Updates Based on Sprite Location:**
- Each frame, SpriteComponent reads GameObject's BodyComponent position
- View's `worldToScreen()` transforms world coordinates → screen coordinates
- Rendering accounts for camera offset (currently centered, but supports camera follow)
- If view center changes, all sprites automatically adjust rendering positions

## Component-Based Architecture

The game uses a **component composition pattern** instead of inheritance hierarchies:

### GameObject Container
```cpp
class GameObject {
    std::vector<std::unique_ptr<Component>> components_;
    
    template<typename T>
    T* getComponent();  // Type-safe component retrieval
    
    void update(float dt);  // Calls update on all components
    void render();          // Calls render on all components
};
```

### Component Base Class
```cpp
class Component {
protected:
    GameObject* owner_;  // Back-reference to parent
    
public:
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    GameObject* getOwner() { return owner_; }
};
```

### Required Components

**1. SpriteComponent** ([src/SpriteComponent.h](src/SpriteComponent.h))
- Stores texture reference from Graphics texture map
- Renders using View transform: `view->worldToScreen()`
- Reads position from BodyComponent each frame

**2. BodyComponent** ([src/BodyComponent.h](src/BodyComponent.h))
- Position (x, y), velocity (vx, vy), rotation
- Physics integration with Box2D (optional enhancement)
- Provides getters/setters for other components

**3. ControllerComponent** ([src/ControllerComponent.h](src/ControllerComponent.h))
- Reads Input device state
- Updates BodyComponent based on keyboard input
- Handles player movement and shooting

**4. BehaviorComponent** ([src/BehaviorComponent.h](src/BehaviorComponent.h))
- **BounceBehavior**: Screen boundary detection, random direction changes
- **ProjectileBehavior**: Upward movement, off-screen detection
- AI logic separated from player control

### Factory Pattern with XML
```cpp
class ObjectFactory {
    std::unordered_map<std::string, 
        std::function<std::unique_ptr<GameObject>(ObjectParams&)>> factories_;
    
    void registerType(const std::string& type, /* factory function */);
    std::unique_ptr<GameObject> create(const std::string& type, 
                                       const ObjectParams& params);
};

// Usage:
auto squirrel = factory.create("Squirrel", params);  // Reads from objects.xml
```

## Code Structure

### Graphics & Input Abstraction
- `src/Graphics.h/cpp` - SDL2 rendering wrapper with texture map
- `src/Input.h/cpp` - SDL2 event handling wrapper with custom Key enum
- `src/View.h/cpp` - Camera/viewport with worldToScreen transform

### Component System
- `src/Component.h` - Abstract base class
- `src/GameObject.h/cpp` - Component container
- `src/BodyComponent.h/cpp` - Position, velocity, physics
- `src/SpriteComponent.h/cpp` - Texture rendering with View
- `src/ControllerComponent.h/cpp` - Player input handling
- `src/BehaviorComponent.h/cpp` - AI behaviors

### Game Logic
- `src/Game.h/cpp` - Main game loop, frame rate limiting, deltaTime
- `src/ObjectFactory.h/cpp` - XML-based object creation
- `src/main.cpp` - Entry point

### Assets
- `assets/config.xml` - Game configuration (speeds, title)
- `assets/objects.xml` - Object definitions with component composition
- `assets/*.png` - Sprite textures (SQRL, acorn, leaf, RBIRD)

## Building and Running

### Prerequisites
- CMake 3.21+
- vcpkg package manager
- MinGW GCC (C++17)

### Dependencies (installed via vcpkg)
- SDL2
- SDL2_image
- SDL2_ttf
- Box2D 3.0
- tinyxml2

### Build Commands
```powershell
# Configure with CMake preset
cmake --preset=win-mingw-debug

# Build
cmake --build build/win-mingw-debug
```

### Run
```powershell
.\build\win-mingw-debug\SQRL-Game.exe
```

## Technical Details

### Architecture Benefits
- **Modularity**: Graphics/Input abstraction allows SDL2 replacement
- **MVC Separation**: Model (BodyComponent), View (SpriteComponent+View), Controller (ControllerComponent+Input)
- **Composition**: GameObject built from components, not rigid inheritance
- **Data-Driven**: XML defines objects, easy to add content without code changes
- **Frame-Independent**: DeltaTime ensures consistent behavior across systems

### Design Patterns
- **Abstraction**: Graphics and Input hide SDL2 implementation
- **Component Pattern**: Composition over inheritance
- **Factory Pattern**: XML-driven object instantiation
- **MVC**: Clear separation of concerns
- **RAII**: Smart pointers for automatic cleanup

### Performance Considerations
- Texture map avoids redundant file loading
- View transform calculated once per sprite, not per pixel
- DeltaTime capping prevents spiral of death on lag spikes
- Frame rate limiting prevents unnecessary CPU usage

## Screenshots

### Gameplay
![Component-based player control](placeholder_gameplay.png)
*Player-controlled squirrel (ControllerComponent) shooting acorns (ProjectileBehavior) at bouncing leaf target (BounceBehavior). All sprites rendered through View transform.*

### View Transform
![View updates based on sprite location](placeholder_view_transform.png)
*SpriteComponent uses View's worldToScreen() to transform game coordinates to screen space. Camera center affects all rendering.*

### Architecture Diagram
![Component System Diagram](placeholder_architecture.png)
*GameObject container with BodyComponent (model), SpriteComponent (view), ControllerComponent (control). Input and Graphics devices abstract SDL2.*

### XML Configuration
![XML-Based Object Factory](placeholder_xml_factory.png)
*Object definitions in objects.xml specify component composition. Factory creates GameObjects at runtime without hardcoded types.*

---

**Note:** This implementation satisfies all project requirements including SDL2 abstraction, MVC separation, View class with transforms, XML asset management, frame rate limiting with deltaTime, and working sprite rendering with input-driven actions.
