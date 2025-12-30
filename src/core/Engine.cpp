#include "Engine.h"
#include "Input.h"
#include "Log.h"
#include "Time.h"
#include "../render/Render.h"
#include "../render/Camera.h"

namespace Core 
{
    Engine::Engine()
        : m_camera(70.0f, 1280.0f / 720.0f, 0.1f, 500.0f)
    {
        Logger::init("engine.log");

        m_camera.setPosition(glm::vec3(40.0f, 80.0f, 40.0f));
        m_camera.setRotation(-45.0f, -30.0f);
        m_camera.setMode(CameraMode::FPS);

        Platform::WindowProps props;
        props.title = "Voxel Engine";

        if (!m_window.initialize(props)) {
            LOG_ENGINE_ERROR("Window initialization failed");
        }

        Render::Renderer::init();  // now BEFORE Game is constructed

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

            // 1. Gather input events from OS / GLFW
            m_window.pollEvents();

            Render::Renderer::beginFrame();

            // 2. Use input this frame
            m_game->onUpdate(Time::deltaTime(), m_camera, m_window);
            m_game->onRender(m_camera, Time::deltaTime());

            Render::Renderer::endFrame();
            m_window.swapBuffers();

            // 3. Commit input state for next frame
            Core::Input::update();
        }
    }
}