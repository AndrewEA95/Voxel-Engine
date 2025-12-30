#define GL_SILENCE_DEPRECATION

#include "Camera.h"
#include "../core/Input.h"
#include "../core/Log.h"
#include "Ray.h"

#include "../../extern/glfw/include/GLFW/glfw3.h"

namespace Render 
{

    Camera::Camera(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
        : m_fov(fovDegrees),
        m_aspect(aspectRatio),
        m_near(nearPlane),
        m_far(farPlane)
    {}

    void Camera::update(float dt)
    {
        if (m_mode == CameraMode::FPS)
            updateFPS(dt);
        else
            updateEditor(dt);
    }

    void Camera::updateFPS(float dt)
    {
        // Mouse look
        float dx = Core::Input::mouseDeltaX();
        float dy = Core::Input::mouseDeltaY();

        const float sensitivity = 0.1f;

        m_yaw   += dx * sensitivity;
        m_pitch += dy * sensitivity;

        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

        // Movement
        glm::vec3 forward = getForward();
        glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));

        const float speed = 10.0f;   // <-- THIS is the missing variable

        // Horizontal movement
        if (Core::Input::isKeyDown(GLFW_KEY_W)) m_position += forward * m_movementSpeed * dt;
        if (Core::Input::isKeyDown(GLFW_KEY_S)) m_position -= forward * m_movementSpeed * dt;
        if (Core::Input::isKeyDown(GLFW_KEY_A)) m_position -= right   * m_movementSpeed * dt;
        if (Core::Input::isKeyDown(GLFW_KEY_D)) m_position += right   * m_movementSpeed * dt;

        // Vertical movement (fly mode)
        if (Core::Input::isKeyDown(GLFW_KEY_SPACE))
            m_position.y += speed * dt;

        if (Core::Input::isKeyDown(GLFW_KEY_LEFT_CONTROL))
            m_position.y -= speed * dt;
    }

    void Camera::updateEditor(float dt)
    {
        // --- ORBIT (RMB) ---
        if (Core::Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
        {
            float dx = Core::Input::mouseDeltaX();
            float dy = Core::Input::mouseDeltaY();

            float orbitSpeed = 0.2f;

            // Hold Shift to orbit faster
            if (Core::Input::isKeyDown(GLFW_KEY_LEFT_SHIFT))
                orbitSpeed *= 3.0f;

            m_yaw   += dx * orbitSpeed;
            m_pitch -= dy * orbitSpeed;

            m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
        }

        // --- PAN (MMB) ---
        if (Core::Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
        {
            float dx = Core::Input::mouseDeltaX();
            float dy = Core::Input::mouseDeltaY();

            float panSpeed = 0.01f * m_distance;

            // Hold Shift to pan faster
            if (Core::Input::isKeyDown(GLFW_KEY_LEFT_SHIFT))
                panSpeed *= 3.0f;

            glm::vec3 forward = getForward();
            glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
            glm::vec3 up      = glm::vec3(0, 1, 0);

            m_pivot += (-right * dx * panSpeed) + (up * dy * panSpeed);
        }

        // --- ZOOM (Q/E) ---
        const float zoomSpeed = 10.0f;

        if (Core::Input::isKeyDown(GLFW_KEY_Q))
            m_distance += zoomSpeed * dt;
        if (Core::Input::isKeyDown(GLFW_KEY_E))
            m_distance -= zoomSpeed * dt;

        m_distance = glm::clamp(m_distance, 1.0f, 200.0f);

        float scroll = Core::Input::getScrollDelta(); // You’ll add this in Input
        if (scroll != 0.0f)
        {
            m_distance -= scroll * 1.5f; // adjust sensitivity
            m_distance = glm::clamp(m_distance, 1.0f, 200.0f);
        }

        // --- UPDATE POSITION FROM ORBIT ---
        glm::vec3 dir;
        dir.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        dir.y = sin(glm::radians(m_pitch));
        dir.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        dir = glm::normalize(dir);

        // Camera sits at pivot - forward * distance
        m_position = m_pivot - dir * m_distance;
    }

    void Camera::setPosition(const glm::vec3& pos)
    {
        m_position = pos;
    }

    void Camera::setRotation(float yawDegrees, float pitchDegrees)
    {
        m_yaw = yawDegrees;
        m_pitch = pitchDegrees;
    }

    void Camera::setMode(CameraMode mode)
    {
        m_mode = mode;
    }

    void Camera::setPivot(const glm::vec3& pivot)
    {
        m_pivot = pivot;
    }

    void Camera::setDistance(float dist)
    {
        m_distance = glm::clamp(dist, 1.0f, 200.0f);
    }

    void Camera::processMouseMovement(float deltaX, float deltaY)
    {
        deltaX *= m_sensitivity;
        deltaY *= m_sensitivity;

        m_yaw += deltaX;
        m_pitch += deltaY;

        // Clamp pitch to avoid flipping
        if(m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;

        updateDirectionVectors();
    }

    void Camera::processKeyboard(float dt)
    {
        float speed = 5.0f * dt;

        if (Core::Input::isKeyDown(GLFW_KEY_W)) {
            LOG_ENGINE_TRACE("W pressed — moving forward");
            m_position += m_front * speed;
        }

        if (Core::Input::isKeyDown(GLFW_KEY_S)) {
            LOG_ENGINE_TRACE("S pressed — moving backward");
            m_position -= m_front * speed;
        }

        if (Core::Input::isKeyDown(GLFW_KEY_A)) {
            LOG_ENGINE_TRACE("A pressed — strafing left");
            glm::vec3 right = glm::normalize(glm::cross(m_front, m_up));
            m_position -= right * speed;
        }

        if (Core::Input::isKeyDown(GLFW_KEY_D)) {
            LOG_ENGINE_TRACE("D pressed — strafing right");
            glm::vec3 right = glm::normalize(glm::cross(m_front, m_up));
            m_position += right * speed;
        }

        // Vertical movement (fly mode)
        if (Core::Input::isKeyDown(GLFW_KEY_SPACE))
        {
            m_position.y += speed;
            LOG_ENGINE_TRACE("Fly up");
        }

        if (Core::Input::isKeyDown(GLFW_KEY_LEFT_CONTROL))
        {
            m_position.y -= speed;
            LOG_ENGINE_TRACE("Fly down");
        }

    }

    glm::mat4 Camera::getProjectionMatrix() const
    {
        return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        glm::vec3 front;

        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        glm::vec3 direction = glm::normalize(front);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        return glm::lookAt(m_position, m_position + direction, up);
    }

    glm::mat4 Camera::getViewProjectionMatrix() const
    {
        return getProjectionMatrix() * getViewMatrix();
    }

    void Camera::updateDirectionVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_front = glm::normalize(front);
    }

    Ray Camera::screenPointToRay(float mouseX, float mouseY) const
    {
        float x = (2.0f * mouseX) / m_ViewportWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / m_ViewportHeight;

        glm::vec4 clip = glm::vec4(x, y, -1.0f, 1.0f);

        glm::mat4 invProj = glm::inverse(getProjectionMatrix());
        glm::vec4 eye = invProj * clip;
        eye = glm::vec4(eye.x, eye.y, -1.0f, 0.0f);

        glm::mat4 invView = glm::inverse(getViewMatrix());
        glm::vec3 worldDir = glm::normalize(glm::vec3(invView * eye));

        return Ray{ m_position, worldDir };
    }

    glm::vec3 Camera::getForward() const
    {
        glm::vec3 forward;
        forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        forward.y = sin(glm::radians(m_pitch));
        forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        return glm::normalize(forward);
    }
}