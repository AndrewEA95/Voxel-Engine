#pragma once

#include "../../extern/glm-1.0.2/glm/glm.hpp"

#include "Mesh.h"
#include "Transform.h"
#include "../ecs/Entity.h"

namespace Render
{
    struct RenderObject
    {
        Mesh* mesh = nullptr;
        Transform transform;
        glm::vec3 color = glm::vec3(1.0f);

        bool isStatic = false;

        std::function<void(RenderObject&, float)> onUpdate;

        RenderObject() = default;

        ECS::Entity entity = ECS::INVALID_ENTITY;

        RenderObject(Mesh* m, const glm::vec3& col) : mesh(m), color(col) {}
    };
}