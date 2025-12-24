#pragma once

#include <functional>


struct GLFWwindow;

namespace Platform 
{

    struct WindowProps 
    {
        std::string title = "Voxel Engine";
        uint32_t width  = 1280;
        uint32_t height = 720;
    };

    class Window 
    {
        public:
            Window() = default;
            ~Window();

            void resetMouseState();
            void setCursorLocked(bool locked);
            bool isCursorLocked() const { return m_cursorLocked; }

            bool initialize(const WindowProps& props = WindowProps{});
            void shutdown();

            void pollEvents();
            void swapBuffers();

            bool shouldClose() const;

            GLFWwindow* getNativeHandle() const { return m_window; }

            void setMouseMoveCallback(std::function<void(float, float)> callback);

        private:
            GLFWwindow* m_window = nullptr;
            WindowProps m_props;
            bool m_cursorLocked = true;

            bool initGLFW();
            bool initGLAD();
    };

}