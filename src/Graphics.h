#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>

class Graphics {
public:
    Graphics() = default;
    ~Graphics();

    bool init(const std::string& title, int width, int height);
    void shutdown();

    // Texture management
    bool loadTexture(const std::string& name, const std::string& filePath);
    SDL_Texture* getTexture(const std::string& name);

    // Font management
    bool loadFont(const std::string& path, int size);

    // Drawing operations
    void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void present();
    void drawTexture(SDL_Texture* texture, int x, int y, int w, int h);
    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void drawFilledRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void drawFilledCircle(int centerX, int centerY, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void drawText(const std::string& text, int x, int y, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255);

    // Getters (for components that still need raw SDL access)
    SDL_Renderer* getRenderer() { return renderer_; }
    SDL_Window* getWindow() { return window_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font* font_ = nullptr;
    std::unordered_map<std::string, SDL_Texture*> textures_;
};
