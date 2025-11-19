#ifndef DOOR_H
#define DOOR_H

#include "Object.h"

class Door : public Object
{
public:
    Door(float x, float y, bool isLevel2Door = false);
    void update() override;
    void render() override;
    
    bool isLocked() const { return locked; }
    void unlock() { locked = false; }
    bool isOpened() const { return opened; }
    void open() { opened = true; }
    bool isLevel2Door() const { return level2Door; }
    
private:
    bool locked;
    bool opened;
    bool level2Door;
};

#endif
