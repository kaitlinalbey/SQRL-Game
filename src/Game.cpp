#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <tinyxml2.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>

using namespace std::chrono_literals;

int Game::run() {
    if (!loadConfig("assets/config.xml")) {
        std::cerr << "Using defaults (could not load assets/config.xml)\n";
    }
    if (!init()) {
        std::cerr << "Init failed.\n";
        return 1;
    }

    auto lastTime = std::chrono::steady_clock::now();
    bool running = true;

    while (running) {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Cap dt to prevent large jumps
        if (dt > 0.1f) dt = 0.1f;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        handleInput();
        update(dt);
        render();

        std::this_thread::sleep_for(16ms); // ~60 FPS
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
    root->QueryFloatAttribute("squirrelSpeed", &squirrelSpeed_);
    root->QueryFloatAttribute("acornSpeed", &acornSpeed_);
    root->QueryFloatAttribute("leafSpeedX", &leafSpeedX_);
    root->QueryFloatAttribute("leafSpeedY", &leafSpeedY_);
    std::cout << "Loaded config: title=\"" << title_ << "\"\n";
    return true;
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG | IMG_INIT_WEBP;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image init error: " << IMG_GetError() << "\n";
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

    // Load textures
    SDL_Surface* squirrelSurf = IMG_Load("assets/SQRL.png");
    if (squirrelSurf) {
        squirrelTexture_ = SDL_CreateTextureFromSurface(renderer_, squirrelSurf);
        SDL_FreeSurface(squirrelSurf);
    } else {
        std::cerr << "Failed to load SQRL.png: " << IMG_GetError() << "\n";
    }

    SDL_Surface* acornSurf = IMG_Load("assets/acorn.png");
    if (acornSurf) {
        acornTexture_ = SDL_CreateTextureFromSurface(renderer_, acornSurf);
        SDL_FreeSurface(acornSurf);
    } else {
        std::cerr << "Failed to load acorn.png: " << IMG_GetError() << "\n";
    }

    SDL_Surface* leafSurf = IMG_Load("assets/leaf.webp");
    if (leafSurf) {
        leafTexture_ = SDL_CreateTextureFromSurface(renderer_, leafSurf);
        SDL_FreeSurface(leafSurf);
    } else {
        std::cerr << "Failed to load leaf.webp: " << IMG_GetError() << "\n";
        // Create a simple colored rectangle as fallback for leaf
        leafTexture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, 60, 60);
        if (leafTexture_) {
            SDL_SetRenderTarget(renderer_, leafTexture_);
            SDL_SetRenderDrawColor(renderer_, 34, 139, 34, 255); // Forest green
            SDL_RenderClear(renderer_);
            SDL_SetRenderTarget(renderer_, nullptr);
        }
    }

    if (!squirrelTexture_ || !acornTexture_ || !leafTexture_) {
        std::cerr << "Failed to load required textures\n";
        return false;
    }

    // Create game objects
    squirrel_ = std::make_unique<Squirrel>(400.0f, 50.0f, 80.0f, 80.0f, squirrelSpeed_);
    leaf_ = std::make_unique<Leaf>(400.0f, 500.0f, 60.0f, 60.0f, leafSpeedX_, leafSpeedY_);
    
    std::cout << "Init complete. Squirrel Acorn Game ready!\n";
    return true;
}

void Game::shutdown() {
    if (squirrelTexture_) { SDL_DestroyTexture(squirrelTexture_); squirrelTexture_ = nullptr; }
    if (acornTexture_) { SDL_DestroyTexture(acornTexture_); acornTexture_ = nullptr; }
    if (leafTexture_) { SDL_DestroyTexture(leafTexture_); leafTexture_ = nullptr; }
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }
    IMG_Quit();
    SDL_Quit();
}

void Game::handleInput() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    float dt = 1.0f / 60.0f;
    
    // Move squirrel left/right
    if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) {
        squirrel_->moveLeft(dt);
    }
    if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) {
        squirrel_->moveRight(dt);
    }

    // Shoot acorn with W key or Up arrow
    if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) {
        if (acornCooldown_ <= 0.0f) {
            float acornX = squirrel_->getX() + squirrel_->getWidth() / 2 - acornWidth_ / 2;
            float acornY = squirrel_->getY() + squirrel_->getHeight();
            acorns_.push_back(std::make_unique<Acorn>(acornX, acornY, acornWidth_, acornHeight_, acornSpeed_));
            acornCooldown_ = ACORN_COOLDOWN_TIME;
        }
    }
}

void Game::update(float dt) {
    // Update cooldown timer
    if (acornCooldown_ > 0.0f) {
        acornCooldown_ -= dt;
    }

    // Update squirrel
    squirrel_->update(dt, SCREEN_WIDTH);

    // Update acorns
    for (auto& acorn : acorns_) {
        if (acorn->isActive()) {
            acorn->update(dt);
            
            // Check collision with leaf
            if (acorn->getX() < leaf_->getX() + leaf_->getWidth() &&
                acorn->getX() + acorn->getWidth() > leaf_->getX() &&
                acorn->getY() < leaf_->getY() + leaf_->getHeight() &&
                acorn->getY() + acorn->getHeight() > leaf_->getY()) {
                acorn->setActive(false);
                score_++;
                std::cout << "Hit! Score: " << score_ << "\n";
            }
            
            // Deactivate if off screen
            if (acorn->isOffScreen(SCREEN_HEIGHT)) {
                acorn->setActive(false);
            }
        }
    }

    // Remove inactive acorns
    acorns_.erase(
        std::remove_if(acorns_.begin(), acorns_.end(), 
            [](const std::unique_ptr<Acorn>& a) { return !a->isActive(); }),
        acorns_.end()
    );

    // Update leaf
    leaf_->update(dt, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Game::render() {
    // Clear screen with sky blue background
    SDL_SetRenderDrawColor(renderer_, 135, 206, 235, 255);
    SDL_RenderClear(renderer_);

    // Draw brown branch at top
    SDL_SetRenderDrawColor(renderer_, 139, 69, 19, 255);
    SDL_Rect branch = {0, static_cast<int>(squirrel_->getY() + squirrel_->getHeight()), SCREEN_WIDTH, 20};
    SDL_RenderFillRect(renderer_, &branch);

    // Draw squirrel
    squirrel_->render(renderer_, squirrelTexture_);

    // Draw acorns
    for (const auto& acorn : acorns_) {
        acorn->render(renderer_, acornTexture_);
    }

    // Draw leaf
    leaf_->render(renderer_, leafTexture_);

    // Present the rendered frame
    SDL_RenderPresent(renderer_);
}


