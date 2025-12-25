#pragma once

#include "../../extern/glm-1.0.2/glm/glm.hpp"
#include "Mesh.h"
#include "RenderObject.h"
#include "Transform.h"
#include "../scene/Scene.h"

namespace Render
{
    class Mesh;
    class Shader;
    class Transform;

    class Renderer
    {
        public:
            static void init();
            static void shutdown();

            static void beginFrame();
            static void endFrame();

            static Mesh* getTestMesh();

            static void drawTestTriangle(const glm::mat4& viewProj, float dt);
            static void drawMesh(const Mesh& mesh, const Transform& transform, const glm::mat4& viewProj);
            static void drawScene(const Scene& scene, const glm::mat4& viewProj, ECS::Entity selectedEntity);

            static void drawObject(const RenderObject& obj, const glm::mat4& viewProj);
    };
}