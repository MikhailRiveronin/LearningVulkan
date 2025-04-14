#pragma once

#include "Defines.h"

#include "InputHandler.h"

#include <unordered_map>
#include <vector>

enum class EventType {
    KEY_DOWN,
    KEY_UP,
    LEFT_BUTTON_DOWN,
    MOUSE_MOVE
};

union EventContext {
    i16 mousePos[4];
    u16 keyCodes[4];
};

class Listener {
public:
    virtual void onNotify(EventType type, EventContext context) = 0;

};

class EventManager {
public:
    static void subscribe(EventType eventType, Listener* listener);
    static void unsubscribe(EventType eventType, Listener* listener);
    static void notify(EventType type, EventContext context);

private:
    static std::unordered_map<EventType, std::vector<Listener*>> listeners;
};
