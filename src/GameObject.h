#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "Component.h"

class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject() = default;

    void addComponent(std::unique_ptr<Component> component);
    
    template<typename T>
    T* getComponent() {
        for (auto& comp : components_) {
            if (T* result = dynamic_cast<T*>(comp.get())) {
                return result;
            }
        }
        return nullptr;
    }

    Component* getComponent(const std::string& type);

    void init();
    void update(float dt);
    void render();

    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

private:
    std::string name_;
    std::vector<std::unique_ptr<Component>> components_;
    bool active_ = true;
};
