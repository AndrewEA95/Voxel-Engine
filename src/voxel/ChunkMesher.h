#pragma once

#include "Chunk.h"
#include "../core/ChunkManager.h"
#include "../render/Mesh.h"
#include "../render/RenderObject.h"

class ChunkMesher
{
    public:
        static Render::RenderObject buildMesh(const Chunk& chunk, const ChunkManager& manager);

        static void addQuad(
            std::vector<float>& vertices,
            std::vector<uint32_t>& indices,
            const glm::vec3& origin,
            const glm::vec3& u,
            const glm::vec3& v,
            const glm::vec3& color);
};