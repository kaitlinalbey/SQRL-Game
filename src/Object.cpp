#include "Object.h"
#include "Engine.h"
#include <SDL.h>
#include <iostream>

Object::Object(float x, float y, float width, float height) 
    : x(x), y(y), width(width), height(height) {}

Object::~Object() {
    // Virtual destructor implementation
}

void Object::update() {
    // Base class update - does nothing by default
    // Derived classes can override this for specific behavior
}

void Object::render() 
{
    // SDL_Renderer* renderer = Engine::E->getRenderer();
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // SDL_Rect rect = {(int)x, (int)y, (int)width, (int)height};
    // SDL_RenderFillRect(renderer, &rect);
    // // std::cout << "Object rendered at (" << x << ", " << y << ")" << std::endl;
    Engine::drawRect(x, y, width, height, 255, 0, 0, 255);
}
