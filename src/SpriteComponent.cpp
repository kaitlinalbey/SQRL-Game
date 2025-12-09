#include "SpriteComponent.h"
#include "GameObject.h"
#include "BodyComponent.h"
#include "View.h"

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

void SpriteComponent::render(const View* view) {
    if (!texture_ || !renderer_) return;
    
    auto* body = owner_->getComponent<BodyComponent>();
    if (!body) return;
    
    // Transform world coordinates to screen coordinates using the view
    int screenX, screenY;
    if (view) {
        view->worldToScreen(body->getX(), body->getY(), screenX, screenY);
    } else {
        screenX = static_cast<int>(body->getX());
        screenY = static_cast<int>(body->getY());
    }
    
    SDL_Rect destRect = {
        screenX,
        screenY,
        static_cast<int>(body->getWidth()),
        static_cast<int>(body->getHeight())
    };
    
    SDL_RenderCopy(renderer_, texture_, nullptr, &destRect);
}
