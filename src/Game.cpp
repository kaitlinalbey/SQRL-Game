#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf init error: " << TTF_GetError() << "\n";
        return false;
    }

    // Try to load a system font (simplified approach)
    font_ = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);
    if (!font_) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
        // Continue anyway, we'll handle null font in drawText
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

    SDL_Surface* leafSurf = IMG_Load("assets/leaf.png");
    if (leafSurf) {
        leafTexture_ = SDL_CreateTextureFromSurface(renderer_, leafSurf);
        SDL_FreeSurface(leafSurf);
    } else {
        std::cerr << "Failed to load leaf.png: " << IMG_GetError() << "\n";
    }

    if (!squirrelTexture_ || !acornTexture_ || !leafTexture_) {
        std::cerr << "Failed to load required textures\n";
        return false;
    }

    // Create game objects
    squirrel_ = std::make_unique<Squirrel>(400.0f, 50.0f, 80.0f, 80.0f, squirrelSpeed_);
    leaf_ = std::make_unique<Leaf>(400.0f, 500.0f, 90.0f, 90.0f, leafSpeedX_, leafSpeedY_);
    
    std::cout << "Init complete. Squirrel Acorn Game ready!\n";
    return true;
}

void Game::shutdown() {
    if (font_) { TTF_CloseFont(font_); font_ = nullptr; }
    if (squirrelTexture_) { SDL_DestroyTexture(squirrelTexture_); squirrelTexture_ = nullptr; }
    if (acornTexture_) { SDL_DestroyTexture(acornTexture_); acornTexture_ = nullptr; }
    if (leafTexture_) { SDL_DestroyTexture(leafTexture_); leafTexture_ = nullptr; }
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }
    TTF_Quit();
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
    if (!gameOver_ && !gameWon_ && (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP])) {
        if (acornCooldown_ <= 0.0f && nutsRemaining_ > 0) {
            float acornX = squirrel_->getX() + squirrel_->getWidth() / 2 - acornWidth_ / 2;
            float acornY = squirrel_->getY() + squirrel_->getHeight();
            acorns_.push_back(std::make_unique<Acorn>(acornX, acornY, acornWidth_, acornHeight_, acornSpeed_));
            acornCooldown_ = ACORN_COOLDOWN_TIME;
            nutsRemaining_--;
            
            if (nutsRemaining_ <= 0) {
                gameOver_ = true;
                std::cout << "Game Over! You ran out of nuts!\n";
            }
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
            
            // Check collision with leaf (smaller hitbox - 60% of actual size)
            float leafHitboxShrink = 0.2f;
            float leafHitX = leaf_->getX() + leaf_->getWidth() * leafHitboxShrink;
            float leafHitY = leaf_->getY() + leaf_->getHeight() * leafHitboxShrink;
            float leafHitW = leaf_->getWidth() * (1.0f - 2 * leafHitboxShrink);
            float leafHitH = leaf_->getHeight() * (1.0f - 2 * leafHitboxShrink);
            
            if (acorn->getX() < leafHitX + leafHitW &&
                acorn->getX() + acorn->getWidth() > leafHitX &&
                acorn->getY() < leafHitY + leafHitH &&
                acorn->getY() + acorn->getHeight() > leafHitY) {
                acorn->setActive(false);
                hits_++;
                score_++;
                std::cout << "Hit! Points: " << hits_ << "/" << HITS_TO_WIN << "\n";
                
                if (hits_ >= HITS_TO_WIN) {
                    gameWon_ = true;
                    std::cout << "You Win! You hit the leaf " << HITS_TO_WIN << " times!\n";
                } else {
                    // Respawn leaf at random position in bottom half
                    float newX = static_cast<float>(rand() % (SCREEN_WIDTH - 90));
                    float newY = static_cast<float>((SCREEN_HEIGHT / 2) + rand() % (SCREEN_HEIGHT / 2 - 90));
                    float newSpeedX = (rand() % 2 == 0 ? 1 : -1) * (150.0f + rand() % 100);
                    float newSpeedY = (rand() % 2 == 0 ? 1 : -1) * (100.0f + rand() % 100);
                    leaf_ = std::make_unique<Leaf>(newX, newY, 90.0f, 90.0f, newSpeedX, newSpeedY);
                }
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

    // Draw acorn icons for remaining nuts (top left)
    int acornIconSize = 25;
    for (int i = 0; i < nutsRemaining_; i++) {
        SDL_Rect iconRect = {10 + i * (acornIconSize + 5), 10, acornIconSize, acornIconSize};
        if (acornTexture_) {
            SDL_RenderCopy(renderer_, acornTexture_, nullptr, &iconRect);
        }
    }

    // Draw UI text
    drawText("Points: " + std::to_string(hits_) + "/" + std::to_string(HITS_TO_WIN), SCREEN_WIDTH - 130, 10);
    
    if (gameOver_) {
        drawText("GAME OVER!", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2);
    } else if (gameWon_) {
        drawText("YOU WIN!", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2);
    }

    // Present the rendered frame
    SDL_RenderPresent(renderer_);
}

void Game::drawText(const std::string& text, int x, int y) {
    if (!font_) return; // No font loaded, skip rendering
    
    SDL_Color color = {255, 255, 255, 255}; // White text
    SDL_Surface* surface = TTF_RenderText_Solid(font_, text.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to render text: " << TTF_GetError() << "\n";
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer_, texture, nullptr, &destRect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

