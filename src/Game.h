#pragma once
#include <memory>
#include <string>
#include <vector>
#include <SDL.h>
#include "Squirrel.h"
#include "Acorn.h"
#include "Leaf.h"

class Game {
public:
    int run();
private:
    bool init();
    void shutdown();
    void update(float dt);
    void render();
    void handleInput();
    bool loadConfig(const std::string& path);

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* squirrelTexture_ = nullptr;
    SDL_Texture* acornTexture_ = nullptr;
    SDL_Texture* leafTexture_ = nullptr;

    std::unique_ptr<Squirrel> squirrel_;
    std::vector<std::unique_ptr<Acorn>> acorns_;
    std::unique_ptr<Leaf> leaf_;

    // Configuration values
    float squirrelSpeed_ = 300.0f;
    float acornSpeed_ = 400.0f;
    float acornWidth_ = 30.0f;
    float acornHeight_ = 30.0f;
    float leafSpeedX_ = 200.0f;
    float leafSpeedY_ = 150.0f;

    float acornCooldown_ = 0.0f;
    static constexpr float ACORN_COOLDOWN_TIME = 1.0f;

    int score_ = 0;
    std::string title_ = "Squirrel Acorn Game";
    
    // Rendering constants
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
};
