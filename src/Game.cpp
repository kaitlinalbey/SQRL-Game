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
#include "ObjectFactory.h"
#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "ControllerComponent.h"
#include "BehaviorComponent.h"

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
    std::vector<std::pair<std::string, std::string>> textureFiles = {
        {"SQRL", "assets/SQRL.png"},
        {"acorn", "assets/acorn.png"},
        {"leaf", "assets/leaf.png"}
    };
    
    for (const auto& [name, path] : textureFiles) {
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (surf) {
            textures_[name] = SDL_CreateTextureFromSurface(renderer_, surf);
            SDL_FreeSurface(surf);
        } else {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
        }
    }

    // Register object types and create game objects
    registerObjectTypes();
    createGameObjects();
    
    std::cout << "Init complete. Squirrel Acorn Game ready!\n";
    return true;
}

void Game::shutdown() {
    if (font_) { TTF_CloseFont(font_); font_ = nullptr; }
    for (auto& [name, texture] : textures_) {
        if (texture) SDL_DestroyTexture(texture);
    }
    textures_.clear();
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::registerObjectTypes() {
    auto& factory = ObjectFactory::instance();
    
    // Register Squirrel
    factory.registerType("Squirrel", [this](const ObjectParams& params) {
        auto obj = std::make_unique<GameObject>("Squirrel");
        obj->addComponent(std::make_unique<BodyComponent>(params.x, params.y, params.width, params.height));
        auto sprite = std::make_unique<SpriteComponent>("SQRL", renderer_);
        sprite->setTexture(textures_["SQRL"]);
        obj->addComponent(std::move(sprite));
        obj->addComponent(std::make_unique<ControllerComponent>(params.speed, SCREEN_WIDTH));
        return obj;
    });
    
    // Register Leaf
    factory.registerType("Leaf", [this](const ObjectParams& params) {
        auto obj = std::make_unique<GameObject>("Leaf");
        auto body = std::make_unique<BodyComponent>(params.x, params.y, params.width, params.height);
        body->setVelocity(params.velocityX, params.velocityY);
        obj->addComponent(std::move(body));
        auto sprite = std::make_unique<SpriteComponent>("leaf", renderer_);
        sprite->setTexture(textures_["leaf"]);
        obj->addComponent(std::move(sprite));
        obj->addComponent(std::make_unique<BounceBehavior>(SCREEN_WIDTH, SCREEN_HEIGHT));
        return obj;
    });
    
    // Register Acorn
    factory.registerType("Acorn", [this](const ObjectParams& params) {
        auto obj = std::make_unique<GameObject>("Acorn");
        auto body = std::make_unique<BodyComponent>(params.x, params.y, params.width, params.height);
        body->setVelocity(0, params.speed);
        obj->addComponent(std::move(body));
        auto sprite = std::make_unique<SpriteComponent>("acorn", renderer_);
        sprite->setTexture(textures_["acorn"]);
        obj->addComponent(std::move(sprite));
        obj->addComponent(std::make_unique<ProjectileBehavior>(SCREEN_HEIGHT));
        return obj;
    });
    
    factory.loadFromXML("assets/objects.xml");
}

void Game::createGameObjects() {
    auto& factory = ObjectFactory::instance();
    
    // Create squirrel
    ObjectParams squirrelParams;
    squirrelParams.x = 400.0f;
    squirrelParams.y = 50.0f;
    squirrelParams.width = 80.0f;
    squirrelParams.height = 80.0f;
    squirrelParams.speed = squirrelSpeed_;
    squirrel_ = factory.create("Squirrel", squirrelParams);
    squirrel_->init();
    
    // Create leaf
    ObjectParams leafParams;
    leafParams.x = 400.0f;
    leafParams.y = 500.0f;
    leafParams.width = 90.0f;
    leafParams.height = 90.0f;
    leafParams.velocityX = leafSpeedX_;
    leafParams.velocityY = leafSpeedY_;
    leaf_ = factory.create("Leaf", leafParams);
    leaf_->init();
}

GameObject* Game::spawnAcorn(float x, float y) {
    ObjectParams acornParams;
    acornParams.x = x;
    acornParams.y = y;
    acornParams.width = acornWidth_;
    acornParams.height = acornHeight_;
    acornParams.speed = acornSpeed_;
    
    auto acorn = ObjectFactory::instance().create("Acorn", acornParams);
    acorn->init();
    acorns_.push_back(std::move(acorn));
    return acorns_.back().get();
}

void Game::handleInput() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    float dt = 1.0f / 60.0f;
    
    auto* controller = squirrel_->getComponent<ControllerComponent>();
    if (controller) {
        // Move squirrel left/right
        if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) {
            controller->moveLeft(dt);
        }
        if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) {
            controller->moveRight(dt);
        }
    }

    // Shoot acorn with W key or Up arrow
    if (!gameOver_ && !gameWon_ && (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP])) {
        if (acornCooldown_ <= 0.0f && nutsRemaining_ > 0) {
            auto* squirrelBody = squirrel_->getComponent<BodyComponent>();
            if (squirrelBody) {
                float acornX = squirrelBody->getX() + squirrelBody->getWidth() / 2 - acornWidth_ / 2;
                float acornY = squirrelBody->getY() + squirrelBody->getHeight();
                spawnAcorn(acornX, acornY);
                acornCooldown_ = ACORN_COOLDOWN_TIME;
                nutsRemaining_--;
                
                if (nutsRemaining_ <= 0) {
                    gameOver_ = true;
                    std::cout << "Game Over! You ran out of nuts!\n";
                }
            }
        }
    }
}

void Game::update(float dt) {
    // Update cooldown timer
    if (acornCooldown_ > 0.0f) {
        acornCooldown_ -= dt;
    }

    // Update game objects
    squirrel_->update(dt);
    leaf_->update(dt);

    auto* leafBody = leaf_->getComponent<BodyComponent>();
    
    // Update acorns
    for (auto& acorn : acorns_) {
        if (!acorn->isActive()) continue;
        
        acorn->update(dt);
        
        auto* acornBody = acorn->getComponent<BodyComponent>();
        auto* projectile = acorn->getComponent<ProjectileBehavior>();
        
        if (acornBody && leafBody) {
            // Check collision with leaf (smaller hitbox - 60% of actual size)
            float leafHitboxShrink = 0.2f;
            float leafHitX = leafBody->getX() + leafBody->getWidth() * leafHitboxShrink;
            float leafHitY = leafBody->getY() + leafBody->getHeight() * leafHitboxShrink;
            float leafHitW = leafBody->getWidth() * (1.0f - 2 * leafHitboxShrink);
            float leafHitH = leafBody->getHeight() * (1.0f - 2 * leafHitboxShrink);
            
            if (acornBody->getX() < leafHitX + leafHitW &&
                acornBody->getX() + acornBody->getWidth() > leafHitX &&
                acornBody->getY() < leafHitY + leafHitH &&
                acornBody->getY() + acornBody->getHeight() > leafHitY) {
                acorn->setActive(false);
                hits_++;
                score_++;
                std::cout << "Hit! Points: " << hits_ << "/" << HITS_TO_WIN << "\n";
                
                if (hits_ >= HITS_TO_WIN) {
                    gameWon_ = true;
                    std::cout << "You Win! You hit the leaf " << HITS_TO_WIN << " times!\n";
                } else {
                    // Respawn leaf at random position
                    ObjectParams leafParams;
                    leafParams.x = static_cast<float>(rand() % (SCREEN_WIDTH - 90));
                    leafParams.y = static_cast<float>((SCREEN_HEIGHT / 2) + rand() % (SCREEN_HEIGHT / 2 - 90));
                    leafParams.width = 90.0f;
                    leafParams.height = 90.0f;
                    leafParams.velocityX = (rand() % 2 == 0 ? 1 : -1) * (150.0f + rand() % 100);
                    leafParams.velocityY = (rand() % 2 == 0 ? 1 : -1) * (100.0f + rand() % 100);
                    leaf_ = ObjectFactory::instance().create("Leaf", leafParams);
                    leaf_->init();
                }
            }
        }
        
        // Deactivate if off screen
        if (projectile && projectile->isOffScreen()) {
            acorn->setActive(false);
        }
    }

    // Remove inactive acorns
    acorns_.erase(
        std::remove_if(acorns_.begin(), acorns_.end(), 
            [](const std::unique_ptr<GameObject>& a) { return !a->isActive(); }),
        acorns_.end()
    );
}

void Game::render() {
    // Clear screen with sky blue background
    SDL_SetRenderDrawColor(renderer_, 135, 206, 235, 255);
    SDL_RenderClear(renderer_);

    // Draw brown branch at top
    auto* squirrelBody = squirrel_->getComponent<BodyComponent>();
    if (squirrelBody) {
        SDL_SetRenderDrawColor(renderer_, 139, 69, 19, 255);
        SDL_Rect branch = {0, static_cast<int>(squirrelBody->getY() + squirrelBody->getHeight()), SCREEN_WIDTH, 20};
        SDL_RenderFillRect(renderer_, &branch);
    }

    // Render game objects
    squirrel_->render();
    
    for (const auto& acorn : acorns_) {
        acorn->render();
    }
    
    leaf_->render();

    // Draw acorn icons for remaining nuts (top left)
    int acornIconSize = 25;
    for (int i = 0; i < nutsRemaining_; i++) {
        SDL_Rect iconRect = {10 + i * (acornIconSize + 5), 10, acornIconSize, acornIconSize};
        if (textures_["acorn"]) {
            SDL_RenderCopy(renderer_, textures_["acorn"], nullptr, &iconRect);
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

