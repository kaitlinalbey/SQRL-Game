#pragma once
#include <string>
#include <unordered_map>
#include <SDL.h>


class ImageDevice {
public:
    // Load texture from image file with optional source rectangle
    static bool load(const std::string& name, const std::string& imagePath);
    static bool load(const std::string& name, const std::string& imagePath, SDL_Rect srcRect);
    
    // Get texture by name
    static SDL_Texture* get(const std::string& name);
    
    // Cleanup all textures
    static void cleanup();
    
    // Check if texture exists
    static bool exists(const std::string& name);

private:
    static std::unordered_map<std::string, SDL_Texture*> textures;
};
