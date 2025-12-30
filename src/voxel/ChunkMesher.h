#pragma once

#include "Chunk.h"
#include "../core/ChunkManager.h"
#include "../render/Mesh.h"
#include "../render/RenderObject.h"

class ChunkMesher
{
    public:
        static Render::RenderObject buildMesh(const Chunk& chunk,
                                          const ChunkManager& manager);

        // thread-safe raw mesh builder
        static void buildMeshRaw(const Chunk& chunk,
                                const ChunkManager& manager,
                                std::vector<float>& outVertices,
                                std::vector<uint32_t>& outIndices);

        // main-thread GPU mesh builder
        static Render::RenderObject buildMeshFromRaw(const Chunk& chunk,
                                                    const std::vector<float>& vertices,
                                                    const std::vector<uint32_t>& indices);

        static void addQuad(
            std::vector<float>& vertices,
            std::vector<uint32_t>& indices,
            const glm::vec3& origin,
            const glm::vec3& u,
            const glm::vec3& v,
            const glm::vec3& color);
};