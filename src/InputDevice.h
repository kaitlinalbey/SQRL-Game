#pragma once
#include <unordered_set>
#include <SDL.h>

class InputDevice {
public:
    // Process SDL events to update key states
    static void process(const SDL_Event& event);
    
    // Check if a key is currently pressed
    static bool isKeyDown(SDL_Keycode key);
    

private:
    static std::unordered_set<SDL_Keycode> pressedKeys;      // Keys currently held down
};
