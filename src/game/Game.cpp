#define GL_SILENCE_DEPRECATION

#include "Game.h"
#include "../voxel/Chunk.h"
#include "../voxel/ChunkMesher.h"
#include "../core/Input.h"
#include "../core/Log.h"
#include "../render/Render.h"
#include "../ecs/Components.h"
#include "../ecs/Hierarchy.h"
#include "../ecs/Systems.h"

#include "../render/Raycast.h"
#include "../../extern/glfw/include/GLFW/glfw3.h"
#include <OpenGL/gl.h>

Game::Game()
{
    // --- Chunk entity ---
    ECS::Entity chunkEntity = m_Registry.createEntity();
    m_Registry.addComponent<ECS::TagComponent>(
        chunkEntity,
        ECS::TagComponent{ "Chunk_0_0_0" }
    );
    m_Registry.addComponent<ECS::TransformComponent>(
        chunkEntity,
        ECS::TransformComponent{}
    );

    Chunk chunk({0,0,0});
    chunk.generateTestData();

    Render::RenderObject chunkObj = ChunkMesher::buildMesh(chunk);
    chunkObj.isStatic = true;
    chunkObj.entity = chunkEntity;        // ECS -> Scene mapping
    m_Scene.addObject(chunkObj);

    // --- Cube A ---
    ECS::Entity cubeAEntity = m_Registry.createEntity();
    m_Registry.addComponent<ECS::TagComponent>(
        cubeAEntity,
        ECS::TagComponent{ "Cube_A" }
    );
    m_Registry.addComponent<ECS::TransformComponent>(
        cubeAEntity,
        ECS::TransformComponent{}
    );

    // ECS transform for Cube A
    auto& cubeATransform = m_Registry
        .getComponent<ECS::TransformComponent>(cubeAEntity)
        .transform;
    cubeATransform.setPosition(glm::vec3(0.0f, 2.0f, -3.0f));

    Render::RenderObject cubeA;
    cubeA.mesh = Render::Renderer::getTestMesh();
    cubeA.color = glm::vec3(0.2f, 0.8f, 0.3f);
    cubeA.entity = cubeAEntity;

    // --- Cube B ---
    ECS::Entity cubeBEntity = m_Registry.createEntity();
    m_Registry.addComponent<ECS::TagComponent>(
        cubeBEntity,
        ECS::TagComponent{ "Cube_B" }
    );
    m_Registry.addComponent<ECS::TransformComponent>(
        cubeBEntity,
        ECS::TransformComponent{}
    );

    // ECS transform for Cube B: local offset behind A, smaller
    auto& cubeBTransform = m_Registry
        .getComponent<ECS::TransformComponent>(cubeBEntity)
        .transform;
    cubeBTransform.setPosition(glm::vec3(0.0f, 0.0f, -1.0f));   // 1 unit behind A
    cubeBTransform.setScale(glm::vec3(0.5f));                   // half size

    Render::RenderObject cubeB;
    cubeB.mesh = Render::Renderer::getTestMesh();
    cubeB.color = glm::vec3(0.3f, 0.5f, 1.0f);
    cubeB.entity = cubeBEntity;

    // Add to scene
    m_Scene.addObject(cubeA);
    m_Scene.addObject(cubeB);

    // --- Hierarchy: make Cube B a child of Cube A ---
    ECS::Attach(m_Registry, cubeBEntity, cubeAEntity);
}

void Game::onUpdate(float dt, Render::Camera& camera, Platform::Window& window)
{
    static bool cursorLocked = true;

    if (Core::Input::isKeyPressed(GLFW_KEY_ESCAPE))
    {
        cursorLocked = !cursorLocked;
        window.setCursorLocked(cursorLocked);
    }

    camera.update(dt);

    // Left click to pick
    if (Core::Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        float mouseX = Core::Input::getMouseX();
        float mouseY = Core::Input::getMouseY();

        Ray ray = camera.screenPointToRay(mouseX, mouseY);

        float closest = FLT_MAX;
        ECS::Entity hit = ECS::INVALID_ENTITY;

        for (auto& obj : m_Scene.getObjects())
        {
            ECS::Entity e = obj.entity;
            if (!m_Registry.hasComponent<ECS::TransformComponent>(e))
                continue;

            auto& tc = m_Registry.getComponent<ECS::TransformComponent>(e).transform;

            glm::vec3 min(-0.5f);
            glm::vec3 max( 0.5f);

            float dist;
            if (intersectRayAABB(ray, tc.getWorldMatrix(), min, max, dist))
            {
                if (dist < closest)
                {
                    closest = dist;
                    hit = e;
                }
            }
        }

        static double lastClickTime = 0.0;
        double now = glfwGetTime();

        bool doubleClick = false;

        if (Core::Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            if (now - lastClickTime < 0.25) // 250ms double-click window
                doubleClick = true;

            lastClickTime = now;
        }

        if (doubleClick && hit != ECS::INVALID_ENTITY)
        {
            auto& tc = m_Registry.getComponent<ECS::TransformComponent>(hit).transform;
            glm::vec3 pos = tc.getWorldPosition();

            camera.setPivot(pos);
            camera.setDistance(5.0f); // zoom in close
        }

        m_selected = hit;

        if (hit != ECS::INVALID_ENTITY)
        {
            auto& tag = m_Registry.getComponent<ECS::TagComponent>(hit);
            LOG_ENGINE_INFO("Selected: " + tag.name);

            // New: move camera pivot to selected object
            auto& tc = m_Registry.getComponent<ECS::TransformComponent>(hit).transform;
            glm::vec3 worldPos = tc.getWorldPosition(); // or extract from world matrix
            camera.setPivot(worldPos);
        }
    }

    if (Core::Input::isKeyPressed(GLFW_KEY_TAB))
    {
        if (camera.getMode() == CameraMode::FPS)
        {
            camera.setMode(CameraMode::Editor);
            window.setCursorLocked(false);
            LOG_ENGINE_INFO("Switched to EDITOR mode");
        }
        else
        {
            camera.setMode(CameraMode::FPS);
            window.setCursorLocked(true);
            LOG_ENGINE_INFO("Switched to FPS mode");
        }
    }

    // Animate Cube A rotation in ECS
    {
        ECS::Entity cubeAEntity = 2; // or store this in a member variable
        if (m_Registry.hasComponent<ECS::TransformComponent>(cubeAEntity))
        {
            auto& tA = m_Registry.getComponent<ECS::TransformComponent>(cubeAEntity).transform;
            glm::vec3 rot = tA.getRotation();
            rot.y += dt * 0.5f;
            tA.setRotation(rot);
        }
    }

    // Animate Cube B scale in ECS
    {
        ECS::Entity cubeBEntity = 3; // or store this in a member variable
        if (m_Registry.hasComponent<ECS::TransformComponent>(cubeBEntity))
        {
            static float t = 0.0f;
            t += dt;

            auto& tB = m_Registry.getComponent<ECS::TransformComponent>(cubeBEntity).transform;

            float scale = 0.5f + sin(t) * 0.25f;
            tB.setScale(glm::vec3(scale));
        }
    }

    // Prevent Cube B from inheriting rotation from Cube A
    {
        ECS::Entity cubeBEntity = 3; // or store this in a member variable
        if (m_Registry.hasComponent<ECS::TransformComponent>(cubeBEntity))
        {
            auto& tB = m_Registry.getComponent<ECS::TransformComponent>(cubeBEntity).transform;
            tB.setRotation(glm::vec3(0.0f)); // lock rotation to zero
        }
    }

    // Per-object updates (for non-static objects)
    for (auto& obj : m_Scene.getObjects())
    {
        if (obj.isStatic)
            continue;

        if (obj.onUpdate)
            obj.onUpdate(obj, dt);
    }

    // Compute ECS world transforms
    ECS::ComputeWorldTransforms(m_Registry);

    // Sync ECS → Scene using world matrices and explicit entity mapping
    for (auto& obj : m_Scene.getObjects())
    {
        ECS::Entity e = obj.entity;
        if (m_Registry.hasComponent<ECS::TransformComponent>(e))
        {
            auto& tc = m_Registry.getComponent<ECS::TransformComponent>(e);
            obj.transform.setWorldMatrix(tc.transform.getWorldMatrix());
        }
    }

    // Debug: tag listing
    if (Core::Input::isKeyPressed(GLFW_KEY_O))
    {
        m_Registry.view<ECS::TagComponent>(
            [&](ECS::Entity e, ECS::TagComponent& tag)
            {
                LOG_ENGINE_INFO("Entity " + std::to_string(e) + " has tag: " + tag.name);
            }
        );
    }
}

void Game::onRender(const Render::Camera& camera, float dt)
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw world
    Render::Renderer::drawScene(m_Scene, camera.getViewProjectionMatrix(), m_selected);

    // Draw crosshair only in FPS mode
    if (camera.getMode() == CameraMode::FPS)
    {
        // Create a tiny cube a bit in front of the camera
        Render::RenderObject crosshair;
        crosshair.mesh   = Render::Renderer::getTestMesh();
        crosshair.color  = glm::vec3(1.0f, 1.0f, 1.0f);
        crosshair.isStatic = true;      // it doesn't animate
        crosshair.entity   = ECS::INVALID_ENTITY; // not part of ECS

        glm::vec3 camPos = camera.getPosition();
        glm::vec3 camFwd = camera.getForward();

        glm::mat4 t = glm::mat4(1.0f);
        t = glm::translate(t, camPos + camFwd * 1.5f);        // 1.5 units in front
        t = glm::scale(t, glm::vec3(0.03f));                  // very small

        crosshair.transform.setWorldMatrix(t);

        // Draw just this object with same VP
        Render::Renderer::drawObject(crosshair, camera.getViewProjectionMatrix());
    }
}