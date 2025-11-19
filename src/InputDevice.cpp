#include "InputDevice.h"
#include <iostream>

// Static member definitions
std::unordered_set<SDL_Keycode> InputDevice::pressedKeys;


void InputDevice::process(const SDL_Event& event) {
    if(event.type == SDL_KEYDOWN)
        pressedKeys.insert(event.key.keysym.sym);
    else if(event.type == SDL_KEYUP)
        pressedKeys.erase(event.key.keysym.sym);
}

bool InputDevice::isKeyDown(SDL_Keycode key) {
    return pressedKeys.find(key) != pressedKeys.end();
}