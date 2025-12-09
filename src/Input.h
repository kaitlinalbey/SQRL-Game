#pragma once

#include <SDL.h>
#include <unordered_map>
#include <string>

enum class Key {
    LEFT, RIGHT, UP, DOWN,
    A, D, W, S,
    SPACE, ENTER, ESCAPE,
    UNKNOWN
};

enum class MouseButton {
    LEFT,
    RIGHT,
    MIDDLE,
    UNKNOWN
};

class Input {
public:
    Input() = default;

    // Call this each frame to update input state
    void update();
    
    // Process SDL events (returns false if quit event received)
    bool processEvents();

    // Keyboard queries
    bool isKeyDown(Key key) const;
    bool isKeyPressed(Key key) const;   // Just pressed this frame
    bool isKeyReleased(Key key) const;  // Just released this frame

    // Mouse queries
    bool isMouseButtonDown(MouseButton button) const;
    bool isMouseButtonPressed(MouseButton button) const;
    bool isMouseButtonReleased(MouseButton button) const;
    void getMousePosition(int& x, int& y) const;

    // Quit
    bool quitRequested() const { return quitRequested_; }

private:
    SDL_Scancode keyToScancode(Key key) const;
    Uint8 mouseButtonToSDL(MouseButton button) const;

    const Uint8* keyboardState_ = nullptr;
    Uint8 prevKeyboardState_[SDL_NUM_SCANCODES] = {};
    
    Uint32 mouseState_ = 0;
    Uint32 prevMouseState_ = 0;
    int mouseX_ = 0;
    int mouseY_ = 0;
    
    bool quitRequested_ = false;
};
