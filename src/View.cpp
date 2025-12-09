#include "View.h"
#include <algorithm>

View::View(float x, float y, float width, float height)
    : centerX_(x), centerY_(y), width_(width), height_(height) {
}

void View::worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const {
    // Convert world coordinates to screen coordinates relative to view center
    float offsetX = worldX - (centerX_ - width_ / 2.0f);
    float offsetY = worldY - (centerY_ - height_ / 2.0f);
    
    screenX = static_cast<int>(offsetX);
    screenY = static_cast<int>(offsetY);
}

void View::setCenter(float x, float y) {
    centerX_ = x;
    centerY_ = y;
    
    if (hasBounds_) {
        clampToBounds();
    }
}

void View::setSize(float width, float height) {
    width_ = width;
    height_ = height;
}

void View::move(float dx, float dy) {
    centerX_ += dx;
    centerY_ += dy;
    
    if (hasBounds_) {
        clampToBounds();
    }
}

void View::followTarget(float targetX, float targetY, float smoothing) {
    // Smoothly move camera towards target
    float dx = (targetX - centerX_) * smoothing;
    float dy = (targetY - centerY_) * smoothing;
    
    centerX_ += dx;
    centerY_ += dy;
    
    if (hasBounds_) {
        clampToBounds();
    }
}

void View::setBounds(float minX, float minY, float maxX, float maxY) {
    hasBounds_ = true;
    minX_ = minX;
    minY_ = minY;
    maxX_ = maxX;
    maxY_ = maxY;
}

void View::clampToBounds() {
    if (!hasBounds_) return;
    
    float halfWidth = width_ / 2.0f;
    float halfHeight = height_ / 2.0f;
    
    centerX_ = std::max(minX_ + halfWidth, std::min(centerX_, maxX_ - halfWidth));
    centerY_ = std::max(minY_ + halfHeight, std::min(centerY_, maxY_ - halfHeight));
}
