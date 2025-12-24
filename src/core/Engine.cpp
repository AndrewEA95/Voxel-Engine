#include "Engine.h"
#include "Input.h"
#include "Log.h"
#include "Time.h"
#include "../render/Render.h"

namespace Core 
{
    Engine::Engine()
        : m_camera(70.0f, 1280.0f / 720.0f, 0.1f, 500.0f)
    {
        Logger::init("engine.log");

        Platform::WindowProps props;
        props.title = "Voxel Engine";

        if (!m_window.initialize(props)) {
            LOG_ENGINE_ERROR("Window initialization failed");
        }

        Render::Renderer::init();  // now BEFORE Game is constructed

        m_window.setMouseMoveCallback([this](float dx, float dy) 
        {
            m_camera.processMouseMovement(dx, dy);
            LOG_ENGINE_TRACE("Mouse dx: " + std::to_string(dx) +
                             " dy: " + std::to_string(dy));
        });

        m_game = std::make_unique<Game>();  // Game sees a valid test mesh now
    }

    Engine::~Engine()
    {
        m_game.reset();
        Render::Renderer::shutdown();
        Logger::shutdown();
    }

    void Engine::run()
    {
        while (!m_window.shouldClose())
        {
            Time::update();
            Core::Input::update();
            m_window.pollEvents();

            Render::Renderer::beginFrame();

            m_game->onUpdate(Time::deltaTime(), m_camera, m_window);
            m_game->onRender(m_camera, Time::deltaTime());

            Render::Renderer::endFrame();
            m_window.swapBuffers();
        }
    }
}