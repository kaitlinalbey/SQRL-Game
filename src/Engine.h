#pragma once
#include <vector>
#include <memory>
#include <string>
#include <SDL.h>
#include "Object.h"

struct View {
    int x{0};
    int y{0};
};

class Engine {
public:
    static Engine* E;
    Engine();
    ~Engine();

    // Core engine methods
    void addObject(Object* obj);
    void setView(int x, int y);
    void update();
    SDL_Renderer* getRenderer(){return renderer;}
    static void drawRect(float x, float y, float width, float height, int r, int g, int b, int a=255);
    static void drawImage(float x, float y, float width, float height, const std::string& textureName);
    // Draw a frame from a horizontal sprite strip (expects 4 frames across)
    // Draw a frame from a sprite sheet. Specify frameIndex and optionally columns/rows (defaults to 2x2).
    static void drawImage(float x, float y, float width, float height, const std::string& textureName, int frameIndex, int cols = 2, int rows = 2);
    static void drawText(const std::string& text, float x, float y, int size, int r, int g, int b, int a=255);

    Object* getObject(int index){return objects[index].get();}
    Object* getLastObject(){return getObject(objects.size()-1);}
    void showGameOver() { gameOver = true; }
    bool isGameOver() const { return gameOver; }
    void showYouLose() { youLose = true; }
    bool isYouLose() const { return youLose; }
    void setLevel2() { level2 = true; }
    bool isLevel2() const { return level2; }
    void clearObjects() { objects.clear(); }
private:
    std::vector<std::unique_ptr<Object>> objects;
    SDL_Window* window;
    SDL_Renderer* renderer;
    View view;
    bool gameOver;
    bool youLose;
    bool level2;
    // Internal methods
    void processInput();
    void updateObjects();
    void render();
};
