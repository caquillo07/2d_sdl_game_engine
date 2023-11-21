//
// Created by hector on 11/20/23.
//

#ifndef KEY_PRESSED_EVENT_H
#define KEY_PRESSED_EVENT_H

#include <SDL2/SDL.h>
#include "../event_bus/event.h"

class KeyPressedEvent : public Event {
public:
    SDL_Keycode key;
    KeyPressedEvent(const SDL_Keycode key): key(key) {
    }
};

#endif //KEY_PRESSED_EVENT_H
