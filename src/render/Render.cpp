#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>

#include <iostream>

#include "Transform.h"

#include "Render.h"
#include "RenderObject.h"
#include "Shader.h"
#include "../scene/Scene.h"
#include "Mesh.h"

namespace Render
{
    static Shader* s_TestShader = nullptr;
    static Mesh* s_TestMesh = nullptr;

    static RenderObject cubeA;
    static RenderObject cubeB;

    

    void Renderer::init()
    {
        // --- GPU State --
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

        // --- Load Shader ---
        s_TestShader = new Shader(
            "../assets/shaders/basic.vert",
            "../assets/shaders/basic.frag"
        );

        // --- Cube Mesh Data ---
        std::vector<float> vertices = {
            // positions
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f
        };

        std::vector<uint32_t> indices = {
            // back face
            0, 1, 2, 2, 3, 0,
            // front face
            4, 5, 6, 6, 7, 4,
            // left face
            0, 4, 7, 7, 3, 0,
            // right face
            1, 5, 6, 6, 2, 1,
            // bottom face
            0, 1, 5, 5, 4, 0,
            // top face
            3, 2, 6, 6, 7, 3
        };

        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" }
        };

        // --- Creat Mesh ---
        s_TestMesh = new Mesh(vertices, indices, layout);


        // --- Creat RenderObjects ---
        cubeA = RenderObject(s_TestMesh, glm::vec3(0.2f, 0.8f, 0.3f)); //green
        cubeB = RenderObject(s_TestMesh, glm::vec3(0.3f, 0.5f, 1.0f));

        cubeA.transform.setPosition(glm::vec3(-1.0f, 0.0f, 0.0f)); // left cube
        cubeB.transform.setPosition(glm::vec3(1.0f, 0.0f, 0.0f)); // right cube
        cubeB.transform.setScale(glm::vec3(0.5f)); //smaller cube

    }

    void Renderer::shutdown()
    {
        delete s_TestMesh;
        delete s_TestShader;
        s_TestMesh = nullptr;
        s_TestShader = nullptr;
    }

    void Renderer::beginFrame()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::endFrame()
    {
        // Future: post-processing, UI, etc.
    }

    void Renderer::drawMesh(const Mesh& mesh, const Transform& transform, const glm::mat4& viewProj)
    {
        s_TestShader->bind();

        // Use ECS world matrix instead of local model matrix
        glm::mat4 model = transform.getWorldMatrix();

        s_TestShader->setUniformMat4("u_ViewProj", viewProj);
        s_TestShader->setUniformMat4("u_Model", model);

        mesh.draw();
    }

    void Renderer::drawScene(const Scene& scene,
                         const glm::mat4& viewProj,
                         ECS::Entity selectedEntity)
{
    // --- Draw all objects normally ---
    for (auto& obj : scene.getObjects())
    {
        if (!obj.mesh)
            continue;

        s_TestShader->bind();
        s_TestShader->setUniformVec3("u_Color", obj.color);
        drawMesh(*obj.mesh, obj.transform, viewProj);
    }

    // --- Optional: highlight selected object ---
    if (selectedEntity != ECS::INVALID_ENTITY)
    {
        try
        {
            const auto& obj = scene.getObjectByEntity(selectedEntity);

            if (obj.mesh)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_DEPTH_TEST);

                s_TestShader->bind();
                s_TestShader->setUniformVec3("u_Color", glm::vec3(1.0f, 1.0f, 0.0f));
                s_TestShader->setUniformMat4("u_Model", obj.transform.getWorldMatrix());
                s_TestShader->setUniformMat4("u_ViewProj", viewProj);

                obj.mesh->draw();

                glEnable(GL_DEPTH_TEST);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        catch (...)
        {
            // Entity disappeared — ignore safely
        }
    }
}
    Mesh* Renderer::getTestMesh()
    {
        return s_TestMesh;
    }

    void Renderer::drawObject(const RenderObject& obj, const glm::mat4& viewProj)
    {
        s_TestShader->bind();

        // Use ECS/world transform like drawMesh does
        glm::mat4 model = obj.transform.getWorldMatrix();

        s_TestShader->setUniformMat4("u_ViewProj", viewProj);
        s_TestShader->setUniformMat4("u_Model", model);
        //s_TestShader->setUniformVec3("u_Color", obj.color);

        obj.mesh->draw();
    }

} // namespace Render
