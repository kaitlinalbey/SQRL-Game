#pragma once
#include "Component.h"

// Base behavior component
class BehaviorComponent : public Component {
public:
    virtual ~BehaviorComponent() = default;
};

// Bouncing behavior for the leaf
class BounceBehavior : public BehaviorComponent {
public:
    BounceBehavior(int screenWidth, int screenHeight);
    
    std::string getType() const override { return "BounceBehavior"; }
    void update(float dt) override;

private:
    int screenWidth_;
    int screenHeight_;
};

// Projectile behavior for acorns
class ProjectileBehavior : public BehaviorComponent {
public:
    ProjectileBehavior(int screenHeight);
    
    std::string getType() const override { return "ProjectileBehavior"; }
    void update(float dt) override;
    
    bool isOffScreen() const { return offScreen_; }

private:
    int screenHeight_;
    bool offScreen_ = false;
};
