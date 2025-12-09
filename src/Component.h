#pragma once
#include <string>

class GameObject;

class Component {
public:
    virtual ~Component() = default;
    virtual void init() {}
    virtual void update(float dt) {}
    virtual void render() {}
    virtual std::string getType() const = 0;
    
    void setOwner(GameObject* owner) { owner_ = owner; }
    GameObject* getOwner() const { return owner_; }
    GameObject& parent() { return *owner_; }
    const GameObject& parent() const { return *owner_; }

protected:
    GameObject* owner_ = nullptr;
};
