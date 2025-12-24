#pragma once

#include "Registry.h"
#include "Components.h"
#include "../render/Transform.h"

namespace ECS
{
    inline void ComputeWorldTransforms(Registry& registry)
    {
        // First, find all root entities (no parent)
        std::vector<Entity> roots;

        registry.view<TransformComponent>([&](Entity e, TransformComponent& tc)
        {
            if (!registry.hasComponent<ParentComponent>(e))
                roots.push_back(e);
        });

        // Recursively compute world trnasforms
        std::function<void(Entity, const glm::mat4&)> recurse =
            [&](Entity e, const glm::mat4& parentWorld)
        {
            auto& tc = registry.getComponent<TransformComponent>(e);

            glm::mat4 local = tc.transform.getMatrix();
            glm::mat4 world = parentWorld * local;

            tc.transform.setWorldMatrix(world);

            if(registry.hasComponent<ChildrenComponent>(e))
            {
                for(Entity child : registry.getComponent<ChildrenComponent>(e).children)
                    recurse(child, world);
            }
        };

        // Start recursion from roots
        for(Entity root : roots)
            recurse(root, glm::mat4(1.0f));
    }
}