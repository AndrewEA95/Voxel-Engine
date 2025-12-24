#include "Time.h"
#include "../../extern/glfw/include/GLFW/glfw3.h"

namespace Core {

float Time::s_deltaTime = 0.0f;
double Time::s_lastTime = 0.0;

void Time::update()
{
    double now = glfwGetTime();
    s_deltaTime = static_cast<float>(now - s_lastTime);
    s_lastTime = now;
}

float Time::deltaTime()
{
    return s_deltaTime;
}

}