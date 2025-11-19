#ifndef KEY_H
#define KEY_H

#include "Object.h"

class Key : public Object
{
public:
    Key(float x, float y);
    void update() override;
    void render() override;
    
    bool isCollected() const { return collected; }
    void collect() { collected = true; }
    
private:
    bool collected;
};

#endif
