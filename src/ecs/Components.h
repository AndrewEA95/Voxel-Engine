#pragma once

#include "../render/Transform.h"
#include "Entity.h"

#include <string>

namespace ECS
{
    struct TagComponent
    {
        std::string name;
    };

    struct TransformComponent
    {
        Render::Transform transform;
    };

    struct SelectedComponent
    {
        bool selected = false;
    };

    struct ParentComponent
    {
        ECS::Entity parent = ECS::INVALID_ENTITY;
    };

    struct ChildrenComponent
    {
        std::vector<ECS::Entity> children;
    };
}