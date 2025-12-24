#pragma once

#include "../../extern/glm-1.0.2/glm/glm.hpp"
#include "../../extern/glm-1.0.2/glm/gtc/matrix_transform.hpp"
#include <algorithm>

namespace Render
{
    class Transform
    {
    public:
        Transform()
            : position(0.0f), rotation(0.0f), scale(1.0f),
              m_WorldMatrix(1.0f)
        {}

        // --- Setters ---
        void setPosition(const glm::vec3& p) { position = p; }
        void setRotation(const glm::vec3& r) { rotation = r; }
        void setScale(const glm::vec3& s)    { scale = s; }
        
        void setFromMatrix(const glm::mat4& m)
        {
            // Extract translation only
            position = glm::vec3(m[3]);
        }

        // --- Getters ---
        const glm::vec3& getPosition() const { return position; }
        const glm::vec3& getRotation() const { return rotation; }
        const glm::vec3& getScale()    const { return scale; }

        // --- Local matrix (your original model matrix) ---
        glm::mat4 getModelMatrix() const
        {
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, position);

            model = glm::rotate(model, rotation.x, glm::vec3(1,0,0));
            model = glm::rotate(model, rotation.y, glm::vec3(0,1,0));
            model = glm::rotate(model, rotation.z, glm::vec3(0,0,1));

            model = glm::scale(model, scale);

            return model;
        }

        // --- Alias for ECS hierarchy system ---
        glm::mat4 getMatrix() const
        {
            return getModelMatrix();
        }

        // --- World transform support ---
        void setWorldMatrix(const glm::mat4& m)
        {
            m_WorldMatrix = m;
        }

        const glm::mat4& getWorldMatrix() const
        {
            return m_WorldMatrix;
        }

    private:
        glm::vec3 position;
        glm::vec3 rotation; // Euler angles in radians
        glm::vec3 scale;

        glm::mat4 m_WorldMatrix; // computed by hierarchy system
    };
}