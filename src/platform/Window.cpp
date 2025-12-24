#define GL_SILENCE_DEPRECATION

#include "Window.h"

#include "../../extern/glad/include/glad/glad.h"
#include "../../extern/glfw/include/GLFW/glfw3.h"

#include "../core/Log.h"
#include"../core/Input.h"

namespace Platform 
{
    static double s_lastMouseX = 0.0;
    static double s_lastMouseY = 0.0;
    static bool s_firstMouse = true;

    static std::function<void(float, float)> s_mouseCallback;

    void Window::setMouseMoveCallback(std::function<void(float, float)> callback) 
    {
        s_mouseCallback = callback;
    }

    static void GLFWMouseCallback(GLFWwindow* window, double xpos, double ypos) 
    {
        if (s_firstMouse) {
            s_lastMouseX = xpos;
            s_lastMouseY = ypos;
            s_firstMouse = false;
        }

        float deltaX = xpos - s_lastMouseX;
        float deltaY = s_lastMouseY - ypos;

        s_lastMouseX = xpos;
        s_lastMouseY = ypos;

        // NEW: store absolute mouse position
        Core::Input::setMousePosition((float)xpos, (float)ypos);

        if (s_mouseCallback)
            s_mouseCallback(deltaX, deltaY);
    }

    static void GLFWErrorCallback(int error, const char* description) {
        LOG_WINDOW_ERROR(std::string("GLFW Error ") + std::to_string(error) + ": " + description);
    }

    Window::~Window() {
        shutdown();
    }

    bool Window::initGLFW() {
        glfwSetErrorCallback(GLFWErrorCallback);

        if (!glfwInit()) {
            LOG_WINDOW_ERROR("Failed to initialize GLFW");
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #if defined(__APPLE__)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        return true;
    }

    bool Window::initGLAD() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            LOG_WINDOW_ERROR("Failed to initialize GLAD");
            return false;
        }

        LOG_WINDOW_INFO(std::string("OpenGL version: ") +
            reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        return true;
    }

    bool Window::initialize(const WindowProps& props) {
        m_props = props;

        LOG_WINDOW_INFO("Initializing window: " + m_props.title);

        if (!initGLFW())
            return false;

        m_window = glfwCreateWindow(
            m_props.width,
            m_props.height,
            m_props.title.c_str(),
            nullptr,
            nullptr
        );

        glfwSetCursorPosCallback(m_window, GLFWMouseCallback);

        glfwSetKeyCallback(m_window, [](GLFWwindow* win, int key, int scancode, int action, int mods) 
        {
            if (action == GLFW_PRESS) 
            {
                Core::Input::setKey(key, true);
                LOG_ENGINE_TRACE("Key pressed: " + std::to_string(key));
            }
            else if (action == GLFW_RELEASE) 
            {
                Core::Input::setKey(key, false);
                LOG_ENGINE_TRACE("Key released: " + std::to_string(key));
            }
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* win, int button, int action, int mods)
        {
            if (action == GLFW_PRESS)
                Core::Input::setMouseButton(button, true);
            else if (action == GLFW_RELEASE)
                Core::Input::setMouseButton(button, false);
        });

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!m_window) {
            LOG_WINDOW_ERROR("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1);

        if (!initGLAD()) {
            glfwDestroyWindow(m_window);
            glfwTerminate();
            return false;
        }

        LOG_WINDOW_INFO("Window initialized successfully");
        return true;
    }

    void Window::shutdown() {
        if (m_window) {
            LOG_WINDOW_INFO("Destroying window");
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        glfwTerminate();
    }

    void Window::pollEvents() {
        glfwPollEvents();
    }

    void Window::swapBuffers() {
        glfwSwapBuffers(m_window);
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_window);
    }

    void Window::resetMouseState()
    {
        s_firstMouse = true;
    }

    void Window::setCursorLocked(bool locked)
    {
        m_cursorLocked = locked;

        glfwSetInputMode(
            m_window,
            GLFW_CURSOR,
            locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
        );

        if (!locked) {
            // Next mouse move after unlock should re-initialize deltas
            resetMouseState();
        }
    }

}