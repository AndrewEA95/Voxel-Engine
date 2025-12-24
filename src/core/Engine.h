#pragma once

#include "../platform/Window.h"
#include "../render/Camera.h"
#include "../game/Game.h"

namespace Core 
{

    class Engine 
    {
        public:
            Engine();
            ~Engine();

            void run();

        private:
            Platform::Window m_window;
            Render::Camera m_camera;
            std::unique_ptr<Game> m_game;
    };

}