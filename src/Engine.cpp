#include "Engine.h"
#include "Object.h"
#include "ImageDevice.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "InputDevice.h"

Engine* Engine::E = nullptr;
static TTF_Font* gameFont = nullptr;

Engine::Engine() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    gameOver = false;
    youLose = false;
    level2 = false;
    
    // Try to load a system font
    gameFont = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 48);
    if (!gameFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    
    E = this;
}
Engine::~Engine() {
    //ImageDevice::cleanup();
    if (gameFont) {
        TTF_CloseFont(gameFont);
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Engine::addObject(Object* obj) {
    if (obj != nullptr) {
        objects.push_back(std::unique_ptr<Object>(obj));
        std::cout << "Added object to engine" << std::endl;
    }
}


void Engine::update() {
    processInput();
    
    // Only update game objects if game is still active
    if (!gameOver && !youLose) {
        updateObjects();
    }
    
    render();
}

void Engine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        InputDevice::process(event);
    }
}

void Engine::updateObjects() {
    // Update all objects
    for (auto& obj : objects) 
        obj->update();
        
}

void Engine::render() {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light gray floor
    SDL_RenderClear(renderer);
    for (auto& obj : objects) 
        obj->render();
    
    // Draw You Lose message if caught by camera
    if (youLose && gameFont) {
        // Draw semi-transparent background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect bgRect = {150, 200, 500, 200};
        SDL_RenderFillRect(renderer, &bgRect);
        
        // Render "YOU LOSE" text
        SDL_Color textColor = {255, 0, 0, 255}; // Red color
        SDL_Surface* textSurface = TTF_RenderText_Solid(gameFont, "YOU LOSE", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {250, 250, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }
    // Draw You Win message if game is over
    else if (gameOver && gameFont) {
        // Draw semi-transparent background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect bgRect = {150, 200, 500, 200};
        SDL_RenderFillRect(renderer, &bgRect);
        
        // Render "YOU WIN!" text
        SDL_Color textColor = {0, 255, 0, 255}; // Green color
        SDL_Surface* textSurface = TTF_RenderText_Solid(gameFont, "YOU WIN!", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {250, 250, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }
    
    SDL_RenderPresent(renderer);

}

// void Engine::drawRect(float x, float y, float width, float height, int r, int g, int b, int a) {
//     SDL_SetRenderDrawColor(Engine::E->renderer, r, g, b, a);
//     SDL_Rect rect = {(int)x, (int)y, (int)width, (int)height};
//     SDL_RenderFillRect(Engine::E->renderer, &rect);
// }


void Engine::drawRect(float x, float y, float width, float height, int r, int g, int b, int a) {
    
    View& view = Engine::E->view;
    SDL_SetRenderDrawColor(Engine::E->renderer, r, g, b, a);
    SDL_Rect rect = {(int)(x-view.x), (int)(y-view.y), (int)width, (int)height};
    SDL_RenderFillRect(Engine::E->renderer, &rect);
}

void Engine::drawImage(float x, float y, float width, float height, const std::string& textureName) {
    View& view = Engine::E->view;
    SDL_Texture* blockTexture = ImageDevice::get(textureName);
    if (!blockTexture) return;
    SDL_Rect rect = {(int)(x-view.x), (int)(y-view.y), (int)width, (int)height};
    SDL_RenderCopy(Engine::E->renderer, blockTexture, nullptr, &rect);
}

void Engine::drawImage(float x, float y, float width, float height, const std::string& textureName, int frameIndex, int cols, int rows) {
    View& view = Engine::E->view;
    SDL_Texture* tex = ImageDevice::get(textureName);
    if (!tex) return;

    int texW = 0, texH = 0;
    SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);
    if (texW <= 0 || texH <= 0) return;

    if (cols <= 0) cols = 1;
    if (rows <= 0) rows = 1;

    int frameW = texW / cols;
    int frameH = texH / rows;
    if (frameW <= 0 || frameH <= 0) return;
    if (frameIndex < 0) frameIndex = 0;
    if (frameIndex >= cols * rows) frameIndex = cols * rows - 1;

    int col = frameIndex % cols;
    int row = frameIndex / cols;
    SDL_Rect src = { col * frameW, row * frameH, frameW, frameH };
    SDL_Rect dst = { (int)(x - view.x), (int)(y - view.y), (int)width, (int)height };
    SDL_RenderCopy(Engine::E->renderer, tex, &src, &dst);
}

void Engine::setView(int x, int y) {
    view.x = x;
    view.y = y;
}

void Engine::drawText(const std::string& text, float x, float y, int size, int r, int g, int b, int a) {
    if (!gameFont) return;
    
    View& view = Engine::E->view;
    SDL_Color textColor = {(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
    SDL_Surface* textSurface = TTF_RenderText_Solid(gameFont, text.c_str(), textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(Engine::E->renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {(int)(x - view.x), (int)(y - view.y), textSurface->w, textSurface->h};
            SDL_RenderCopy(Engine::E->renderer, textTexture, nullptr, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}
