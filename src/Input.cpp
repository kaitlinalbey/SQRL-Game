#include "Input.h"
#include <cstring>

void Input::update() {
    // Store previous keyboard state
    if (keyboardState_) {
        memcpy(prevKeyboardState_, keyboardState_, SDL_NUM_SCANCODES);
    }
    
    // Get current keyboard state
    keyboardState_ = SDL_GetKeyboardState(nullptr);
    
    // Store previous mouse state
    prevMouseState_ = mouseState_;
    
    // Get current mouse state
    mouseState_ = SDL_GetMouseState(&mouseX_, &mouseY_);
}

bool Input::processEvents() {
    quitRequested_ = false;
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quitRequested_ = true;
            return false;
        }
    }
    
    return true;
}

bool Input::isKeyDown(Key key) const {
    if (!keyboardState_) return false;
    SDL_Scancode scancode = keyToScancode(key);
    return keyboardState_[scancode] != 0;
}

bool Input::isKeyPressed(Key key) const {
    if (!keyboardState_) return false;
    SDL_Scancode scancode = keyToScancode(key);
    return keyboardState_[scancode] && !prevKeyboardState_[scancode];
}

bool Input::isKeyReleased(Key key) const {
    if (!keyboardState_) return false;
    SDL_Scancode scancode = keyToScancode(key);
    return !keyboardState_[scancode] && prevKeyboardState_[scancode];
}

bool Input::isMouseButtonDown(MouseButton button) const {
    Uint8 sdlButton = mouseButtonToSDL(button);
    return (mouseState_ & SDL_BUTTON(sdlButton)) != 0;
}

bool Input::isMouseButtonPressed(MouseButton button) const {
    Uint8 sdlButton = mouseButtonToSDL(button);
    return (mouseState_ & SDL_BUTTON(sdlButton)) && !(prevMouseState_ & SDL_BUTTON(sdlButton));
}

bool Input::isMouseButtonReleased(MouseButton button) const {
    Uint8 sdlButton = mouseButtonToSDL(button);
    return !(mouseState_ & SDL_BUTTON(sdlButton)) && (prevMouseState_ & SDL_BUTTON(sdlButton));
}

void Input::getMousePosition(int& x, int& y) const {
    x = mouseX_;
    y = mouseY_;
}

SDL_Scancode Input::keyToScancode(Key key) const {
    switch (key) {
        case Key::LEFT:   return SDL_SCANCODE_LEFT;
        case Key::RIGHT:  return SDL_SCANCODE_RIGHT;
        case Key::UP:     return SDL_SCANCODE_UP;
        case Key::DOWN:   return SDL_SCANCODE_DOWN;
        case Key::A:      return SDL_SCANCODE_A;
        case Key::D:      return SDL_SCANCODE_D;
        case Key::W:      return SDL_SCANCODE_W;
        case Key::S:      return SDL_SCANCODE_S;
        case Key::R:      return SDL_SCANCODE_R;
        case Key::F:      return SDL_SCANCODE_F;
        case Key::T:      return SDL_SCANCODE_T;
        case Key::SPACE:  return SDL_SCANCODE_SPACE;
        case Key::ENTER:  return SDL_SCANCODE_RETURN;
        case Key::ESCAPE: return SDL_SCANCODE_ESCAPE;
        default:          return SDL_SCANCODE_UNKNOWN;
    }
}

Uint8 Input::mouseButtonToSDL(MouseButton button) const {
    switch (button) {
        case MouseButton::LEFT:   return SDL_BUTTON_LEFT;
        case MouseButton::RIGHT:  return SDL_BUTTON_RIGHT;
        case MouseButton::MIDDLE: return SDL_BUTTON_MIDDLE;
        default:                  return 0;
    }
}
