#include "GameObject.h"
#include "SpriteComponent.h"
#include "View.h"

GameObject::GameObject(const std::string& name) : name_(name), active_(true) {
}

void GameObject::addComponent(std::unique_ptr<Component> component) {
    component->setOwner(this);
    components_.push_back(std::move(component));
}

Component* GameObject::getComponent(const std::string& type) {
    for (auto& comp : components_) {
        if (comp->getType() == type) {
            return comp.get();
        }
    }
    return nullptr;
}

void GameObject::init() {
    for (auto& comp : components_) {
        comp->init();
    }
}

void GameObject::update(float dt) {
    if (!active_) return;
    for (auto& comp : components_) {
        comp->update(dt);
    }
}

void GameObject::render() {
    if (!active_) return;
    for (auto& comp : components_) {
        comp->render();
    }
}

void GameObject::render(const View* view) {
    if (!active_) return;
    for (auto& comp : components_) {
        // Check if component is a SpriteComponent and use view-based rendering
        if (auto* sprite = dynamic_cast<SpriteComponent*>(comp.get())) {
            sprite->render(view);
        } else {
            comp->render();
        }
    }
}
