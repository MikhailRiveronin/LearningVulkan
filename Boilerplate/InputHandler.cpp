#include "InputHandler.h"

#include "EventManager.h"
#include "KeyCodes.h"

std::unordered_map<u16, bool> InputHandler::keyboardState = {
    { KEY_CODE_W, false },
    { KEY_CODE_A, false },
    { KEY_CODE_S, false },
    { KEY_CODE_D, false },
    { KEY_CODE_ESC, false }
};

MouseState InputHandler::mouseState;

void InputHandler::processKeystroke(u16 keyCode, bool pressed)
{
    // if (keyboardState[keyCode] != pressed) {
    //     keyboardState[keyCode] = pressed;

        EventContext context;
        context.keyCodes[0] = keyCode;
        EventManager::notify(pressed ? EventType::KEY_DOWN : EventType::KEY_UP, context);
    // }
}

void InputHandler::processLeftMouseButton(i16 x, i16 y)
{
    EventContext context;
    context.mousePos[0] = x;
    context.mousePos[1] = y;
    EventManager::notify(EventType::LEFT_BUTTON_DOWN, context);
}

void InputHandler::processMouseMove(i16 x, i16 y)
{
    EventContext context;
    context.mousePos[0] = x;
    context.mousePos[1] = y;
    EventManager::notify(EventType::MOUSE_MOVE, context);
}
