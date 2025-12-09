#include "Game.h"
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
        // Calculate frame start time
        auto frameStart = std::chrono::steady_clock::now();
        
        // Calculate deltaTime from last frame
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Cap dt to prevent large jumps (e.g., when debugging or window dragging)
        if (dt > 0.1f) dt = 0.1f;

        // Process input events
        input_.update();
        if (!input_.processEvents() || input_.quitRequested()) {
            running = false;
        }

        handleInput();
        update(dt);
        render();

        // Calculate frame duration
        auto frameEnd = std::chrono::steady_clock::now();
        float frameDuration = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();
        
        // Delay if work finished early to maintain target FPS
        if (frameDuration < TARGET_FRAME_TIME) {
            float delayTime = TARGET_FRAME_TIME - frameDuration;
            std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(delayTime));
        }
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
    // Initialize graphics system
    if (!graphics_.init(title_, SCREEN_WIDTH, SCREEN_HEIGHT)) {
        return false;
    }

    // Load font
    if (!graphics_.loadFont("C:\\Windows\\Fonts\\arial.ttf", 24)) {
        std::cerr << "Warning: Failed to load font\n";
    }

    // Load textures
    graphics_.loadTexture("SQRL", "assets/SQRL.png");
    graphics_.loadTexture("acorn", "assets/acorn.png");
    graphics_.loadTexture("leaf", "assets/leaf.png");
    graphics_.loadTexture("RBIRD", "assets/RBIRD.png");

    // Initialize view centered on screen
    view_.setCenter(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
    view_.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Register view as static instance in Graphics for engine-level access
    Graphics::setView(&view_);

    // Register object types (don't create game objects until title screen is dismissed)
    registerObjectTypes();
    
    std::cout << "Init complete. Squirrel Acorn Game ready!\n";
    return true;
}

void Game::shutdown() {
    graphics_.shutdown();
}

void Game::registerObjectTypes() {
    auto& factory = ObjectFactory::instance();
    
    // Register Squirrel
    factory.registerType("Squirrel", [this](const ObjectParams& params) {
        auto obj = std::make_unique<GameObject>("Squirrel");
        obj->addComponent(std::make_unique<BodyComponent>(params.x, params.y, params.width, params.height));
        auto sprite = std::make_unique<SpriteComponent>("SQRL", graphics_.getRenderer());
        sprite->setTexture(graphics_.getTexture("SQRL"));
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
        auto sprite = std::make_unique<SpriteComponent>("leaf", graphics_.getRenderer());
        sprite->setTexture(graphics_.getTexture("leaf"));
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
        auto sprite = std::make_unique<SpriteComponent>("acorn", graphics_.getRenderer());
        sprite->setTexture(graphics_.getTexture("acorn"));
        obj->addComponent(std::move(sprite));
        obj->addComponent(std::make_unique<ProjectileBehavior>(SCREEN_HEIGHT));
        return obj;
    });
    
    // Register RedBlock (fast moving obstacle - red bird)
    factory.registerType("RedBlock", [this](const ObjectParams& params) {
        auto obj = std::make_unique<GameObject>("RedBlock");
        auto body = std::make_unique<BodyComponent>(params.x, params.y, params.width, params.height);
        body->setVelocity(params.velocityX, params.velocityY);
        obj->addComponent(std::move(body));
        auto sprite = std::make_unique<SpriteComponent>("RBIRD", graphics_.getRenderer());
        sprite->setTexture(graphics_.getTexture("RBIRD"));
        obj->addComponent(std::move(sprite));
        obj->addComponent(std::make_unique<BounceBehavior>(SCREEN_WIDTH, SCREEN_HEIGHT));
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
    // Title screen: press SPACE or ENTER to start
    if (gameState_ == GameState::TITLE_SCREEN) {
        if (input_.isKeyPressed(Key::SPACE) || input_.isKeyPressed(Key::ENTER)) {
            gameState_ = GameState::PLAYING;
            createGameObjects();
        }
        return;
    }
    
    // Only handle game input when playing
    if (gameState_ != GameState::PLAYING) return;
    
    float dt = 1.0f / 60.0f;
    
    auto* controller = squirrel_->getComponent<ControllerComponent>();
    if (controller) {
        // Move squirrel left/right
        if (input_.isKeyDown(Key::LEFT) || input_.isKeyDown(Key::A)) {
            controller->moveLeft(dt);
        }
        if (input_.isKeyDown(Key::RIGHT) || input_.isKeyDown(Key::D)) {
            controller->moveRight(dt);
        }
    }

    // Shoot acorn with W key or Up arrow
    if (!gameOver_ && !gameWon_ && (input_.isKeyDown(Key::W) || input_.isKeyDown(Key::UP))) {
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
    if (gameState_ != GameState::PLAYING) return;
    if (gameOver_ || gameWon_) return;
    
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
                std::cout << "Hit! Points: " << hits_ << "/" << hitsToWin_ << "\n";
                
                if (hits_ >= hitsToWin_) {
                    if (currentLevel_ == 1) {
                        levelTransition_ = true;
                        std::cout << "Level 1 Complete! Starting Level 2...\n";
                    } else {
                        gameWon_ = true;
                        std::cout << "You Win! You completed both levels!\n";
                    }
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
    
    // Update red block if in level 2
    if (currentLevel_ == 2 && redBlock_) {
        redBlock_->update(dt);
        
        // Check collision between acorns and red block (hit gives 2 points)
        auto* redBody = redBlock_->getComponent<BodyComponent>();
        if (redBody) {
            for (auto& acorn : acorns_) {
                if (!acorn->isActive()) continue;
                auto* acornBody = acorn->getComponent<BodyComponent>();
                if (acornBody) {
                    if (acornBody->getX() < redBody->getX() + redBody->getWidth() &&
                        acornBody->getX() + acornBody->getWidth() > redBody->getX() &&
                        acornBody->getY() < redBody->getY() + redBody->getHeight() &&
                        acornBody->getY() + acornBody->getHeight() > redBody->getY()) {
                        acorn->setActive(false);
                        hits_ += 2;
                        score_ += 2;
                        std::cout << "Red Ball Hit! +2 Points: " << hits_ << "/" << hitsToWin_ << "\n";
                        
                        // Check if level complete
                        if (hits_ >= hitsToWin_) {
                            gameWon_ = true;
                            std::cout << "You Win! You completed both levels!\n";
                        }
                    }
                }
            }
        }
    }
    
    // Handle level transition
    if (levelTransition_) {
        levelTransition_ = false;
        startLevel2();
    }
}

void Game::render() {
    if (gameState_ == GameState::TITLE_SCREEN) {
        renderTitleScreen();
        return;
    }
    
    // Clear screen with sky blue background
    graphics_.clear(135, 206, 235, 255);

    // Draw brown branch at top
    auto* squirrelBody = squirrel_->getComponent<BodyComponent>();
    if (squirrelBody) {
        graphics_.drawFilledRect(0, static_cast<int>(squirrelBody->getY() + squirrelBody->getHeight()), 
                                SCREEN_WIDTH, 20, 139, 69, 19, 255);
    }

    // Render game objects using view transforms
    squirrel_->render(&view_);
    
    for (const auto& acorn : acorns_) {
        acorn->render(&view_);
    }
    
    leaf_->render(&view_);
    
    // Render red bird sprite if in level 2
    if (currentLevel_ == 2 && redBlock_) {
        redBlock_->render(&view_);
    }

    // Draw acorn icons for remaining nuts (top left)
    int acornIconSize = 25;
    for (int i = 0; i < nutsRemaining_; i++) {
        graphics_.drawTexture(graphics_.getTexture("acorn"), 
                            10 + i * (acornIconSize + 5), 10, acornIconSize, acornIconSize);
    }

    // Draw UI text
    drawText("Level " + std::to_string(currentLevel_), SCREEN_WIDTH / 2 - 40, 10);
    drawText("Points: " + std::to_string(hits_) + "/" + std::to_string(hitsToWin_), SCREEN_WIDTH - 130, 10);
    
    if (gameOver_) {
        drawText("GAME OVER!", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2);
    } else if (gameWon_) {
        drawText("YOU WIN!", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2);
    }

    // Present the rendered frame
    graphics_.present();
}

void Game::drawText(const std::string& text, int x, int y) {
    graphics_.drawText(text, x, y);
}

void Game::renderTitleScreen() {
    // Clear with pale green background
    graphics_.clear(144, 238, 144, 255);
    
    // Game title
    drawText("SQUIRREL ACORN GAME", SCREEN_WIDTH / 2 - 150, 100);
    
    // Game description
    drawText("You're a Squirrel, time to be a menace", SCREEN_WIDTH / 2 - 220, 180);
    drawText("and throw acorns!", SCREEN_WIDTH / 2 - 100, 220);
    
    // Instructions
    drawText("HOW TO PLAY:", SCREEN_WIDTH / 2 - 80, 290);
    drawText("- Move squirrel: Arrow Keys or A/D", 150, 340);
    drawText("- Shoot acorns: W or Up Arrow", 150, 380);
    drawText("- Hit things to gain points", 150, 420);
    drawText("- Get enough points to reach the next level!", 150, 460);
    
    // Play prompt
    drawText("Press SPACE or ENTER to Play", SCREEN_WIDTH / 2 - 180, 520);
    
    graphics_.present();
}

void Game::startLevel2() {
    currentLevel_ = 2;
    hits_ = 0;
    hitsToWin_ = LEVEL2_HITS; // Double the points needed
    nutsRemaining_ = 10 + LEVEL2_EXTRA_NUTS; // Replenish acorns to full + bonus
    
    // Clear existing acorns
    acorns_.clear();
    
    // Respawn leaf with faster speed
    ObjectParams leafParams;
    leafParams.x = 400.0f;
    leafParams.y = 500.0f;
    leafParams.width = 90.0f;
    leafParams.height = 90.0f;
    leafParams.velocityX = leafSpeedX_ * 1.2f; // Slightly faster
    leafParams.velocityY = leafSpeedY_ * 1.2f;
    leaf_ = ObjectFactory::instance().create("Leaf", leafParams);
    leaf_->init();
    
    // Create red circle obstacle (1.25x faster than leaf)
    ObjectParams redParams;
    redParams.x = static_cast<float>(rand() % (SCREEN_WIDTH - 60));
    redParams.y = static_cast<float>((SCREEN_HEIGHT / 2) + rand() % (SCREEN_HEIGHT / 2 - 60));
    redParams.width = 60.0f;
    redParams.height = 60.0f;
    redParams.velocityX = (rand() % 2 == 0 ? 1 : -1) * (leafSpeedX_ * 1.25f + rand() % 50); // 1.25x speed
    redParams.velocityY = (rand() % 2 == 0 ? 1 : -1) * (leafSpeedY_ * 1.25f + rand() % 50);
    redBlock_ = ObjectFactory::instance().create("RedBlock", redParams);
    redBlock_->init();
    
    std::cout << "Level 2 Started! Points needed: " << hitsToWin_ << ", Nuts: " << nutsRemaining_ << "\n";
}

