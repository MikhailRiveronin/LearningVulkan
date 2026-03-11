#pragma once

#include "Defines.h"

#include <unordered_map>

struct MouseState {
    bool left;
    bool middle;
    bool right;
};

class InputHandler {
public:
    static void processKeystroke(u16 code, bool pressed);
    static void processLeftMouseButton(i16 x, i16 y);
    static void processMouseMove(i16 x, i16 y);

private:
    static std::unordered_map<u16, bool> keyboardState;
    static MouseState mouseState;
};
