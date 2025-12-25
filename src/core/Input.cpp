#include "Input.h"
#include <unordered_map>

namespace Core
{
    std::unordered_map<int, bool> Input::s_keys;
    std::unordered_map<int, bool> Input::s_prevKeys;

    float Input::s_mouseDX = 0.0f;
    float Input::s_mouseDY = 0.0f;

    float Input::s_mouseX = 0.0f;
    float Input::s_mouseY = 0.0f;

    std::unordered_map<int, bool> Input::s_mouseButtons;
    std::unordered_map<int, bool> Input::s_prevMouseButtons;

    void Input::Init(){}

    void Input::update()
    {
        s_prevKeys = s_keys;
        s_mouseDX = 0.0f;
        s_mouseDY = 0.0f;
        s_scrollDelta = 0.0f;

    }

    bool Input::isKeyDown(int key)
    {
        return s_keys[key];
    }

    bool Input::isKeyPressed(int key)
    {
        return s_keys[key] && !s_prevKeys[key];
    }

    bool Input::isKeyReleased(int key)
    {
        return !s_keys[key] && s_prevKeys[key];
    }

    void Input::setKey(int key, bool down)
    {
        s_keys[key] = down;
    }

    void Input::setMouseDelta(float dx, float dy)
    {
        s_mouseDX = dx;
        s_mouseDY = dy;
    }

    float Input::mouseDeltaX(){ return s_mouseDX; }
    float Input::mouseDeltaY() { return s_mouseDY; }

    float Input::getMouseX()
    {
        return s_mouseX;
    }

    float Input::getMouseY()
    {
        return s_mouseY;
    }

    void Input::setMousePosition(float x, float y)
    {
        s_mouseX = x;
        s_mouseY = y;
    }

    bool Input::isMouseButtonPressed(int button)
    {
        return s_mouseButtons[button] && !s_prevMouseButtons[button];
    }

    void Input::setMouseButton(int button, bool down)
    {
        s_prevMouseButtons[button] = s_mouseButtons[button];
        s_mouseButtons[button] = down;
    }

    float Input::s_scrollDelta = 0.0f;

    float Input::getScrollDelta() { return s_scrollDelta; }

    void Input::setScrollDelta(float delta)
    {
        s_scrollDelta = delta;
    }
}