#include "Graphics.h"
#include <SDL_image.h>
#include <iostream>

// Initialize static view pointer
View* Graphics::view_ = nullptr;

Graphics::~Graphics() {
    shutdown();
}

bool Graphics::init(const std::string& title, int width, int height) {
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

    window_ = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);
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

    return true;
}

void Graphics::shutdown() {
    if (font_) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }

    for (auto& [name, texture] : textures_) {
        if (texture) SDL_DestroyTexture(texture);
    }
    textures_.clear();

    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool Graphics::loadTexture(const std::string& name, const std::string& filePath) {
    SDL_Surface* surf = IMG_Load(filePath.c_str());
    if (!surf) {
        std::cerr << "Failed to load " << filePath << ": " << IMG_GetError() << "\n";
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surf);
    SDL_FreeSurface(surf);

    if (!texture) {
        std::cerr << "Failed to create texture from " << filePath << ": " << SDL_GetError() << "\n";
        return false;
    }

    // Enable alpha blending for transparency
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    textures_[name] = texture;
    return true;
}

SDL_Texture* Graphics::getTexture(const std::string& name) {
    auto it = textures_.find(name);
    return (it != textures_.end()) ? it->second : nullptr;
}

bool Graphics::loadFont(const std::string& path, int size) {
    font_ = TTF_OpenFont(path.c_str(), size);
    if (!font_) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
        return false;
    }
    return true;
}

void Graphics::clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    SDL_RenderClear(renderer_);
}

void Graphics::present() {
    SDL_RenderPresent(renderer_);
}

void Graphics::drawTexture(SDL_Texture* texture, int x, int y, int w, int h) {
    if (!texture) return;
    SDL_Rect destRect = {x, y, w, h};
    SDL_RenderCopy(renderer_, texture, nullptr, &destRect);
}

void Graphics::drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer_, &rect);
}

void Graphics::drawFilledRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer_, &rect);
}

void Graphics::drawFilledCircle(int centerX, int centerY, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    
    // Draw filled circle using midpoint circle algorithm
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderDrawPoint(renderer_, centerX + x, centerY + y);
            }
        }
    }
}

void Graphics::drawText(const std::string& text, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!font_) return;
    
    SDL_Color color = {r, g, b, a};
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
