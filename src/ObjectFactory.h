#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "GameObject.h"
#include <SDL.h>

struct ObjectParams {
    float x = 0, y = 0;
    float width = 0, height = 0;
    float speed = 0;
    float velocityX = 0, velocityY = 0;
    std::string textureName;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    int screenWidth = 800;
    int screenHeight = 600;
};

class ObjectFactory {
public:
    using CreateFunc = std::function<std::unique_ptr<GameObject>(const ObjectParams&)>;
    
    static ObjectFactory& instance();
    
    void registerType(const std::string& type, CreateFunc func);
    std::unique_ptr<GameObject> create(const std::string& type, const ObjectParams& params);
    
    // Load object definitions from XML
    bool loadFromXML(const std::string& filepath);

private:
    ObjectFactory() = default;
    std::unordered_map<std::string, CreateFunc> creators_;
};
