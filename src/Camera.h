#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"

class Player;

class Camera : public Object
{
public:
    Camera(float x, float y);
    void update() override;
    void render() override;
    
    bool checkPlayerInVision(Player* player);
    
private:
    float angle;           // Current rotation angle of the vision cone
    float angleSpeed;      // Speed of rotation
    float minAngle;        // Minimum angle (left bound)
    float maxAngle;        // Maximum angle (right bound)
    float direction;       // 1 for increasing angle, -1 for decreasing
    float visionRange;     // How far the cone extends
    float visionWidth;     // Width angle of the cone in degrees
    
    // Helper to check if a point is inside the vision cone
    bool isPointInCone(float px, float py);
};

#endif
