#include "ImageDevice.h"
#include "Engine.h"
#include <SDL_image.h>
#include <iostream>

// Static member definitions
std::unordered_map<std::string, SDL_Texture*> ImageDevice::textures;

bool ImageDevice::load(const std::string& name, const std::string& imagePath) {
    // Load the image surface
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        std::cerr << "ImageDevice: Failed to load image '" << imagePath << "': " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Engine::E->getRenderer(), surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        std::cerr << "ImageDevice: Failed to create texture from '" << imagePath << "': " << SDL_GetError() << std::endl;
        return false;
    }
    
    textures[name] = texture;
    std::cout << "ImageDevice: Loaded texture '" << name << "' from '" << imagePath << std::endl;
    
    return true;
}

bool ImageDevice::load(const std::string& name, const std::string& imagePath, SDL_Rect srcRect) {
    // Load the image surface
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        std::cerr << "ImageDevice: Failed to load image '" << imagePath << "': " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Create texture from surface
    // Extract just the rectangle and make a new texture
    SDL_Surface* croppedSurface = SDL_CreateRGBSurfaceWithFormat(0, srcRect.w, srcRect.h, surface->format->BitsPerPixel, surface->format->format);
    if (!croppedSurface) {
        std::cerr << "ImageDevice: Failed to create cropped surface: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }
    if (SDL_BlitSurface(surface, &srcRect, croppedSurface, nullptr) != 0) {
        std::cerr << "ImageDevice: Failed to blit surface: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        SDL_FreeSurface(croppedSurface);
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Engine::E->getRenderer(), croppedSurface);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(croppedSurface);
    if (!texture) {
        std::cerr << "ImageDevice: Failed to create texture from '" << imagePath << "': " << SDL_GetError() << std::endl;
        return false;
    }
    
    textures[name] = texture;
    std::cout << "ImageDevice: Loaded texture '" << name << "' from '" << imagePath << "' region (" << srcRect.x << "," << srcRect.y << "," << srcRect.w << "," << srcRect.h << ")" << std::endl;
    
    return true;
}

SDL_Texture* ImageDevice::get(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return textures[name];
    }
    
    std::cerr << "ImageDevice: Texture '" << name << "' not found!" << std::endl;
    return nullptr;
}



void ImageDevice::cleanup() {
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
    std::cout << "ImageDevice: Cleaned up all textures" << std::endl;
}

bool ImageDevice::exists(const std::string& name) {
    return textures.find(name) != textures.end();
}


