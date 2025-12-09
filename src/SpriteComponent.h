#pragma once
#include "Component.h"
#include <SDL.h>
#include <string>

class View;

class SpriteComponent : public Component {
public:
    SpriteComponent(const std::string& textureName, SDL_Renderer* renderer);
    
    std::string getType() const override { return "SpriteComponent"; }
    
    void setTexture(SDL_Texture* texture) { texture_ = texture; }
    void render() override;  // Legacy render without view
    void render(const View* view);  // Render with view transform

private:
    SDL_Texture* texture_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    std::string textureName_;
};
