#pragma once
#include <iterator>
#include <unordered_map>

namespace Core
{
    class Input
    {
        public:
            static void Init();
            static void update();

            static bool isKeyDown(int key);
            static bool isKeyPressed(int key);
            static bool isKeyReleased(int key);

            static void setKey(int Key, bool down);

            static float mouseDeltaX();
            static float mouseDeltaY();
            static void setMouseDelta(float dx, float dy);

            static float getMouseX();
            static float getMouseY();
            static void setMousePosition(float x, float y);

            static bool isMouseButtonPressed(int button);
            static void setMouseButton(int button, bool down);

            static float getScrollDelta();
            static void setScrollDelta(float delta);

        private:
            static std::unordered_map<int, bool> s_keys;
            static std::unordered_map<int, bool> s_prevKeys;

            static float s_mouseDX;
            static float s_mouseDY;

            static float s_mouseX;
            static float s_mouseY;
            static float s_scrollDelta;

            static std::unordered_map<int, bool> s_mouseButtons;
            static std::unordered_map<int, bool> s_prevMouseButtons;
    };
}