#pragma once

class View {
public:
    View(float x = 0.0f, float y = 0.0f, float width = 800.0f, float height = 600.0f);

    // Transform world coordinates to screen coordinates
    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const;
    
    // Getters
    float getCenterX() const { return centerX_; }
    float getCenterY() const { return centerY_; }
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    
    // Setters
    void setCenter(float x, float y);
    void setSize(float width, float height);
    
    // Camera movement
    void move(float dx, float dy);
    void followTarget(float targetX, float targetY, float smoothing = 1.0f);
    
    // Bounds checking
    void setBounds(float minX, float minY, float maxX, float maxY);
    void clampToBounds();

private:
    float centerX_;
    float centerY_;
    float width_;
    float height_;
    
    // Optional bounds
    bool hasBounds_ = false;
    float minX_, minY_, maxX_, maxY_;
};
