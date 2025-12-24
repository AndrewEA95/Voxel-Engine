#pragma once

#include "Entity.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cassert>

namespace ECS
{
    // Base interface for component storage
    struct IComponentStorage
    {
        virtual ~IComponentStorage() = default;
        virtual void remove(Entity entity) = 0;
    };

    // Templated storage for a single component type
    template<typename T>
    class ComponentStorage : public IComponentStorage
    {
    public:
        T& add(Entity entity, const T& component)
        {
            return m_Components[entity] = component;
        }

        template<typename... Args>
        T& emplace(Entity entity, Args&&... args)
        {
            return m_Components.emplace(entity, T{ std::forward<Args>(args)... }).first->second;
        }

        bool has(Entity entity) const
        {
            return m_Components.find(entity) != m_Components.end();
        }

        T& get(Entity entity)
        {
            auto it = m_Components.find(entity);
            assert(it != m_Components.end() && "Component not found on entity");
            return it->second;
        }

        const T& get(Entity entity) const
        {
            auto it = m_Components.find(entity);
            assert(it != m_Components.end() && "Component not found on entity");
            return it->second;
        }

        void remove(Entity entity) override
        {
            m_Components.erase(entity);
        }

        std::unordered_map<Entity, T>& data() { return m_Components; }

    private:
        std::unordered_map<Entity, T> m_Components;
    };

    class Registry
    {
    public:
        Entity createEntity()
        {
            Entity e = ++m_NextEntity;
            m_Alive[e] = true;
            return e;
        }

        void destroyEntity(Entity entity)
        {
            if (!isAlive(entity))
                return;

            m_Alive.erase(entity);

            // Remove from all component storages
            for (auto& [type, storage] : m_Storages)
            {
                storage->remove(entity);
            }
        }

        bool isAlive(Entity entity) const
        {
            return m_Alive.find(entity) != m_Alive.end();
        }

        template<typename T, typename... Args>
        T& addComponent(Entity entity, Args&&... args)
        {
            auto& storage = getOrCreateStorage<T>();
            return storage.emplace(entity, std::forward<Args>(args)...);
        }

        template<typename T>
        bool hasComponent(Entity entity) const
        {
            auto it = m_Storages.find(std::type_index(typeid(T)));
            if (it == m_Storages.end())
                return false;

            auto* storage = static_cast<ComponentStorage<T>*>(it->second.get());
            return storage->has(entity);
        }

        template<typename T>
        T& getComponent(Entity entity)
        {
            auto& storage = getOrCreateStorage<T>();
            return storage.get(entity);
        }

        template<typename T>
        const T& getComponent(Entity entity) const
        {
            const auto& storage = getOrCreateStorage<T>();
            return storage.get(entity);
        }

        // Simple view for one component type
        template<typename T, typename Func>
        void view(Func func)
        {
            auto& storage = getOrCreateStorage<T>();
            for (auto& [entity, comp] : storage.data())
            {
                if (!isAlive(entity))
                    continue;
                func(entity, comp);
            }
        }

    private:
        template<typename T>
        ComponentStorage<T>& getOrCreateStorage() const
        {
            auto type = std::type_index(typeid(T));
            auto it = m_Storages.find(type);
            if (it == m_Storages.end())
            {
                auto storage = std::make_unique<ComponentStorage<T>>();
                auto* ptr = storage.get();
                const_cast<Registry*>(this)->m_Storages.emplace(type, std::move(storage));
                return *ptr;
            }

            return *static_cast<ComponentStorage<T>*>(it->second.get());
        }

        mutable std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> m_Storages;
        std::unordered_map<Entity, bool> m_Alive;
        Entity m_NextEntity = 0;
    };
}