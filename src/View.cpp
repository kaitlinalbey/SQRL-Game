#include "View.h"
#include <algorithm>
#include <cmath>

View::View(float x, float y, float width, float height)
    : centerX_(x), centerY_(y), width_(width), height_(height) {
}

void View::worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const {
    // Translate world coordinates relative to view center
    float relativeX = worldX - centerX_;
    float relativeY = worldY - centerY_;
    
    // Apply rotation if non-zero
    if (rotation_ != 0.0f) {
        float cosAngle = std::cos(-rotation_);
        float sinAngle = std::sin(-rotation_);
        float rotatedX = relativeX * cosAngle - relativeY * sinAngle;
        float rotatedY = relativeX * sinAngle + relativeY * cosAngle;
        relativeX = rotatedX;
        relativeY = rotatedY;
    }
    
    // Apply scale
    relativeX *= scale_;
    relativeY *= scale_;
    
    // Convert to screen coordinates (center on screen)
    screenX = static_cast<int>(relativeX + width_ / 2.0f);
    screenY = static_cast<int>(relativeY + height_ / 2.0f);
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
