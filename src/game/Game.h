#pragma once
#include "../render/Camera.h"
#include "../platform/Window.h"
#include "../scene/Scene.h"
#include "../ecs/Registry.h"


class Game 
{
    public:
        Game(); 
        void onUpdate(float dt, Render::Camera& camera, Platform::Window& window);
        void onRender(const Render::Camera& camera, float dt);

        ECS::Entity m_selected = ECS::INVALID_ENTITY;

    private:
        ECS::Registry m_Registry;
        Scene m_Scene;
};