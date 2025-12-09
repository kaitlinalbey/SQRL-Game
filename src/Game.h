#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "GameObject.h"
#include "Graphics.h"
#include "Input.h"
#include "View.h"
#include "PhysicsWorld.h"

class Game {
public:
    int run();
private:
    enum class GameState {
        TITLE_SCREEN,
        PLAYING,
        GAME_OVER
    };

    bool init();
    void shutdown();
    void update(float dt);
    void render();
    void renderTitleScreen();
    void handleInput();
    bool loadConfig(const std::string& path);
    void drawText(const std::string& text, int x, int y);
    void registerObjectTypes();
    void createGameObjects();
    GameObject* spawnAcorn(float x, float y);
    void startLevel2();
    void handleCollision(void* bodyA, void* bodyB);

    Graphics graphics_;
    Input input_;
    View view_;
    PhysicsWorld physicsWorld_;

    std::unique_ptr<GameObject> squirrel_;
    std::vector<std::unique_ptr<GameObject>> acorns_;
    std::unique_ptr<GameObject> leaf_;
    std::unique_ptr<GameObject> redBlock_;

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
    int currentLevel_ = 1;
    int hitsToWin_ = 6;
    static constexpr int NUTS_TO_START = 10;
    static constexpr int LEVEL1_HITS = 6;
    static constexpr int LEVEL2_HITS = 12;
    static constexpr int LEVEL2_EXTRA_NUTS = 3;
    bool gameOver_ = false;
    bool gameWon_ = false;
    bool levelTransition_ = false;

    int score_ = 0;
    std::string title_ = "Squirrel Acorn Game";
    GameState gameState_ = GameState::TITLE_SCREEN;
    
    // Rendering constants
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
    
    // Frame rate limiting
    static constexpr int TARGET_FPS = 60;
    static constexpr float TARGET_FRAME_TIME = 1000.0f / TARGET_FPS;  // milliseconds
};
