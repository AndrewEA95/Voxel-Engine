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
              m_WorldMatrix(1.0f), m_HasWorldMatrix(false)
        {}

        // --- Setters (local) ---
        void setPosition(const glm::vec3& p)
        {
            position = p;
            m_HasWorldMatrix = false;
        }

        void setRotation(const glm::vec3& r)
        {
            rotation = r;
            m_HasWorldMatrix = false;
        }

        void setScale(const glm::vec3& s)
        {
            scale = s;
            m_HasWorldMatrix = false;
        }

        // Optional helper if you ever set from a matrix directly (local mode)
        void setFromMatrix(const glm::mat4& m)
        {
            position = glm::vec3(m[3]);
            m_HasWorldMatrix = false;
        }

        // --- Getters ---
        const glm::vec3& getPosition() const { return position; }
        const glm::vec3& getRotation() const { return rotation; }
        const glm::vec3& getScale()    const { return scale; }

        // --- Local model matrix ---
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

        glm::mat4 getMatrix() const { return getModelMatrix(); }

        // --- World transform support (for ECS) ---
        void setWorldMatrix(const glm::mat4& m)
        {
            m_WorldMatrix = m;
            m_HasWorldMatrix = true;
        }

        const glm::mat4& getWorldMatrix() const
        {
            return m_WorldMatrix;
        }

        bool hasWorldMatrix() const { return m_HasWorldMatrix; }

        glm::vec3 getWorldPosition() const
        {
            glm::mat4 world = getWorldMatrix();
            return glm::vec3(world[3]);
        }

    private:
        glm::vec3 position;
        glm::vec3 rotation; // Euler
        glm::vec3 scale;

        glm::mat4 m_WorldMatrix;
        bool      m_HasWorldMatrix;
    };
}