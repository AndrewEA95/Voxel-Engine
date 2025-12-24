#pragma once

#include <vector>
#include "../render/RenderObject.h"

class Scene
{
    public:
        Scene() = default;

        void addObject(const Render::RenderObject& obj)
        {
            m_Objects.push_back(obj);
        }

        std::vector<Render::RenderObject>& getObjects()
        {
            return m_Objects;
        }

        const std::vector<Render::RenderObject>& getObjects() const
        {
            return m_Objects;
        }

        Render::RenderObject& getObjectByEntity(ECS::Entity e) const
        {
            for (auto& obj : m_Objects)
                if (obj.entity == e)
                    return const_cast<Render::RenderObject&>(obj);

            throw std::runtime_error("Entity not found in scene");
        }

    private:
        std::vector<Render::RenderObject> m_Objects;
};