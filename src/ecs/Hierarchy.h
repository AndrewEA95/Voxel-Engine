#pragma once

#include "Entity.h"
#include "Registry.h"
#include "Components.h"

namespace ECS
{
    inline void Attach(Registry& registry, Entity child, Entity parent)
    {
        // Set parent
        registry.addComponent<ParentComponent>(child, ParentComponent{ parent });

        // Add child to parent's list
        if(!registry.hasComponent<ChildrenComponent>(parent))
        {
            registry.addComponent<ChildrenComponent>(parent, ChildrenComponent{});
        }

        auto& children = registry.getComponent<ChildrenComponent>(parent).children;
        children.push_back(child);
    }

    inline void Detach(Registry& registry, Entity child)
    {
        if(!registry.hasComponent<ParentComponent>(child))
        {
            return;
        }

        Entity parent = registry.getComponent<ParentComponent>(child).parent;

        if(registry.hasComponent<ChildrenComponent>(parent))
        {
            auto& children = registry.getComponent<ChildrenComponent>(parent).children;
            children.erase(std::remove(children.begin(), children.end(), child), children.end());
        }

        registry.getComponent<ParentComponent>(child).parent = INVALID_ENTITY;
    }
}