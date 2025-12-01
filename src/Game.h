#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>
#include <SDL_ttf.h>
#include "GameObject.h"

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
    void drawText(const std::string& text, int x, int y);
    void registerObjectTypes();
    void createGameObjects();
    GameObject* spawnAcorn(float x, float y);

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    std::unordered_map<std::string, SDL_Texture*> textures_;
    TTF_Font* font_ = nullptr;

    std::unique_ptr<GameObject> squirrel_;
    std::vector<std::unique_ptr<GameObject>> acorns_;
    std::unique_ptr<GameObject> leaf_;

    // Configuration values
    float squirrelSpeed_ = 300.0f;
    float acornSpeed_ = 400.0f;
    float acornWidth_ = 30.0f;
    float acornHeight_ = 30.0f;
    float leafSpeedX_ = 200.0f;
    float leafSpeedY_ = 150.0f;

    float acornCooldown_ = 0.0f;
    static constexpr float ACORN_COOLDOWN_TIME = 1.0f;

    int nutsRemaining_ = 10;
    int hits_ = 0;
    static constexpr int NUTS_TO_START = 10;
    static constexpr int HITS_TO_WIN = 6;
    bool gameOver_ = false;
    bool gameWon_ = false;

    int score_ = 0;
    std::string title_ = "Squirrel Acorn Game";
    
    // Rendering constants
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
};
