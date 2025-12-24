#pragma once

#include "Ray.h"
#include "../../extern/glm-1.0.2/glm/glm.hpp"
#include "../../extern/glm-1.0.2/glm/gtc/matrix_transform.hpp"

namespace Render 
{

    class Camera 
    {
        public:
            Camera(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);

            void setPosition(const glm::vec3& pos);
            void setRotation(float yawDegrees, float pitchDegrees);
            void processMouseMovement(float deltaX, float deltaY);
            void setSensitivity(float sensitivity) {m_sensitivity = sensitivity;}

            void processKeyboard(float dt);

            const glm::vec3& getPosition() const { return m_position; }
            float getYaw() const { return m_yaw; }
            float getPitch() const { return m_pitch; }

            glm::mat4 getViewMatrix() const;
            glm::mat4 getProjectionMatrix() const;
            glm::mat4 getViewProjectionMatrix() const;

            Ray screenPointToRay(float mx, float my) const;

            void setViewportSize(float width, float height)
            {
                m_ViewportWidth = width;
                m_ViewportHeight = height;
            }

        private:
            void updateDirectionVectors();
            glm::vec3 m_position {0.0f, 0.0f, 3.0f};
            float m_yaw   = -90.0f;  // Facing -Z by default
            float m_pitch = 0.0f;

            glm::vec3 m_front{0.0f, 0.0f, -1.0f};
            glm::vec3 m_up{0.0f, 1.0f, 0.0f};

            float m_fov;
            float m_aspect;
            float m_near;
            float m_far;
            float m_sensitivity = 0.2f;

            float m_ViewportWidth = 1280.0f;
            float m_ViewportHeight = 720.0f;
    };

}