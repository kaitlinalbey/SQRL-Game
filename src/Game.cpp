#include "Game.h"
#include <SDL.h>
#include <box2d/box2d.h>
#include <tinyxml2.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <cmath>

using namespace std::chrono_literals;

int Game::run() {
    if (!loadConfig("assets/config.xml")) {
        std::cerr << "Using defaults (could not load assets/config.xml)\n";
    }
    if (!init()) {
        std::cerr << "Init failed.\n";
        return 1;
    }

    auto start = std::chrono::steady_clock::now();
    float accumulator = 0.0f;
    const float dt = 1.0f / 60.0f;
    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        accumulator += dt;
        while (accumulator >= dt) {
            stepPhysics(dt);
            accumulator -= dt;
        }

        // Spawn blocks at intervals
        spawnTimer_ += dt;
        if (spawnTimer_ >= spawnInterval_) {
            spawnRandomBlock();
            spawnTimer_ = 0.0f;
        }

        render();

        std::this_thread::sleep_for(10ms);
    }

    shutdown();
    return 0;
}

bool Game::loadConfig(const std::string& path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML: " << path << "\n";
        return false;
    }
    auto* root = doc.RootElement();
    if (!root) return false;
    if (const char* t = root->Attribute("title")) title_ = t;
    root->QueryFloatAttribute("gravityY", &gravityY_);
    float spawnRate;
    root->QueryFloatAttribute("spawnRate", &spawnRate);
    spawnInterval_ = 1.0f / spawnRate; // Convert spawns per second to seconds between spawns
    root->QueryFloatAttribute("boxSize", &boxSize_);
    std::cout << "Loaded config: title=\"" << title_ << "\" gravityY=" << gravityY_ 
              << " spawnRate=" << spawnRate << " spawnInterval=" << spawnInterval_ << " boxSize=" << boxSize_ << "\n";
    return true;
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }
    window_ = SDL_CreateWindow(title_.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window_) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }
    renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    // Create world with gravity (negative Y pulls downward in Box2D)
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = B2_LITERAL(b2Vec2){0.0f, -gravityY_};
    worldId_ = b2CreateWorld(&worldDef);

    // Create ground body - position it below where the box will fall
    b2BodyDef groundDef = b2DefaultBodyDef();
    groundDef.position = B2_LITERAL(b2Vec2){0.0f, -4.0f}; // Ground at y=-4 (moved down)
    groundId_ = b2CreateBody(worldId_, &groundDef);
    
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2Polygon groundBox = b2MakeBox(50.0f, 1.0f);
    b2CreatePolygonShape(groundId_, &groundShapeDef, &groundBox);

    // Initialize random number generator
    rng_.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    
    // Create block texture once
    int blockSize = static_cast<int>(boxSize_ * PIXELS_PER_METER);
    blockTexture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, 
                                    SDL_TEXTUREACCESS_TARGET, blockSize, blockSize);
    if (!blockTexture_) {
        std::cerr << "Failed to create block texture: " << SDL_GetError() << "\n";
        return false;
    }
    
    // Draw the block pattern onto the texture
    SDL_SetRenderTarget(renderer_, blockTexture_);
    SDL_SetRenderDrawColor(renderer_, 255, 100, 100, 255); // Red
    SDL_RenderClear(renderer_);
    SDL_SetRenderTarget(renderer_, nullptr);
    
    std::cout << "Init complete. Falling blocks will spawn randomly.\n";
    return true;
}

void Game::shutdown() {
    if (B2_IS_NON_NULL(worldId_)) {
        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
    }
    if (blockTexture_) { SDL_DestroyTexture(blockTexture_); blockTexture_ = nullptr; }
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }
    SDL_Quit();
}

void Game::stepPhysics(float dt) {
    b2World_Step(worldId_, dt, 6);
}

void Game::render() {
    // Clear screen with dark blue background
    SDL_SetRenderDrawColor(renderer_, 20, 20, 30, 255);
    SDL_RenderClear(renderer_);

    // Simple coordinate conversion: Box2D world to screen pixels
    // Screen center is at (400, 300), Box2D origin is at (0, 0)
    auto worldToScreen = [this](b2Vec2 worldPos) -> SDL_Point {
        SDL_Point screenPos;
        screenPos.x = static_cast<int>(SCREEN_WIDTH/2 + worldPos.x * PIXELS_PER_METER);
        screenPos.y = static_cast<int>(SCREEN_HEIGHT/2 - worldPos.y * PIXELS_PER_METER); // Flip Y axis
        return screenPos;
    };

    // Draw ground (static rectangle) - positioned at y=-4, size 100x2
    SDL_SetRenderDrawColor(renderer_, 100, 100, 100, 255); // Gray
    SDL_Rect groundRect;
    groundRect.x = SCREEN_WIDTH/2 - static_cast<int>(50.0f * PIXELS_PER_METER); // Center the 100m wide ground
    groundRect.y = SCREEN_HEIGHT/2 + static_cast<int>((4.0f - 1.0f) * PIXELS_PER_METER); // Ground center at y=-4, top edge at y=-5
    groundRect.w = static_cast<int>(100.0f * PIXELS_PER_METER); // 100m wide
    groundRect.h = static_cast<int>(2.0f * PIXELS_PER_METER); // 2m tall
    SDL_RenderFillRect(renderer_, &groundRect);

    // Draw all falling blocks with rotation using color modulation
    for (const auto& block : fallingBlocks_) {
        if (B2_IS_NON_NULL(block.bodyId)) {
            b2Vec2 blockPos = b2Body_GetPosition(block.bodyId);
            b2Transform transform = b2Body_GetTransform(block.bodyId);
            
            // Convert Box2D transform to screen coordinates
            int centerX = SCREEN_WIDTH/2 + static_cast<int>(blockPos.x * PIXELS_PER_METER);
            int centerY = SCREEN_HEIGHT/2 - static_cast<int>(blockPos.y * PIXELS_PER_METER);
            
            // Get rotation angle from transform (convert to degrees for SDL)
            float angle = atan2(transform.q.s, transform.q.c) * 180.0f / 3.14159f;
            
            // Set color modulation for this block
            SDL_SetTextureColorMod(blockTexture_, block.color.r, block.color.g, block.color.b);
            
            // Draw the rotated texture with color modulation
            int blockSize = static_cast<int>(boxSize_ * PIXELS_PER_METER);
            SDL_Rect destRect = {centerX - blockSize/2, centerY - blockSize/2, blockSize, blockSize};
            SDL_RenderCopyEx(renderer_, blockTexture_, nullptr, &destRect, angle, nullptr, SDL_FLIP_NONE);
        }
    }

    // Present the rendered frame
    SDL_RenderPresent(renderer_);
}

void Game::spawnRandomBlock() {
    // Random X position between -8 and 8 (screen width coverage)
    std::uniform_real_distribution<float> xDist(-8.0f, 8.0f);
    std::uniform_int_distribution<int> colorDist(0, 255);
    
    float x = xDist(rng_);
    
    // Generate random color
    SDL_Color blockColor = {
        static_cast<Uint8>(colorDist(rng_)), // Red
        static_cast<Uint8>(colorDist(rng_)), // Green  
        static_cast<Uint8>(colorDist(rng_)), // Blue
        255 // Alpha (fully opaque)
    };
    
    // Create falling block at top of screen
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = B2_LITERAL(b2Vec2){x, 8.0f}; // Start at top
    b2BodyId blockId = b2CreateBody(worldId_, &bodyDef);
    
    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.density = 1.0f;
    dynamicShapeDef.material.restitution = 0.3f; // Add some bounciness
    b2Polygon dynamicBox = b2MakeBox(boxSize_/2.0f, boxSize_/2.0f); // b2MakeBox uses half-width/half-height
    b2CreatePolygonShape(blockId, &dynamicShapeDef, &dynamicBox);
    
    // Store block with its color
    fallingBlocks_.push_back({blockId, blockColor});
}
