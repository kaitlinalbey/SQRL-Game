#pragma once
#include "Component.h"

class BodyComponent : public Component {
public:
    BodyComponent(float x = 0, float y = 0, float w = 0, float h = 0);
    
    std::string getType() const override { return "BodyComponent"; }
    
    float getX() const { return x_; }
    float getY() const { return y_; }
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    
    void setX(float x) { x_ = x; }
    void setY(float y) { y_ = y; }
    void setPosition(float x, float y) { x_ = x; y_ = y; }
    void setSize(float w, float h) { width_ = w; height_ = h; }
    
    float getVelocityX() const { return velocityX_; }
    float getVelocityY() const { return velocityY_; }
    void setVelocity(float vx, float vy) { velocityX_ = vx; velocityY_ = vy; }
    
    void update(float dt) override;

private:
    float x_, y_;
    float width_, height_;
    float velocityX_ = 0;
    float velocityY_ = 0;
};
