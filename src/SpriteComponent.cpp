#include "SpriteComponent.h"
#include "GameObject.h"
#include "BodyComponent.h"

SpriteComponent::SpriteComponent(const std::string& textureName, SDL_Renderer* renderer)
    : textureName_(textureName), renderer_(renderer) {
}

void SpriteComponent::render() {
    if (!texture_ || !renderer_) return;
    
    auto* body = owner_->getComponent<BodyComponent>();
    if (!body) return;
    
    SDL_Rect destRect = {
        static_cast<int>(body->getX()),
        static_cast<int>(body->getY()),
        static_cast<int>(body->getWidth()),
        static_cast<int>(body->getHeight())
    };
    
    SDL_RenderCopy(renderer_, texture_, nullptr, &destRect);
}
