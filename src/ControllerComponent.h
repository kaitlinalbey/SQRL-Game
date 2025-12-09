#pragma once
#include "Component.h"

class ControllerComponent : public Component {
public:
    ControllerComponent(float speed, int screenWidth);
    
    std::string getType() const override { return "ControllerComponent"; }
    
    void update(float dt) override;
    
    void moveLeft(float dt);
    void moveRight(float dt);

private:
    float speed_;
    int screenWidth_;
};
