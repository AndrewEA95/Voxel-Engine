#include "ChunkMesher.h"
#include "Chunk.h"
#include "Voxel.h"

#include "../../extern/glm-1.0.2/glm/glm.hpp"

using namespace Render;

// Correct vertex positions for each face (4 verts per face)
static const glm::vec3 FACE_VERTS[6][4] =
{
    // +X
    { {1,0,0}, {1,1,0}, {1,1,1}, {1,0,1} },

    // -X
    { {0,0,1}, {0,1,1}, {0,1,0}, {0,0,0} },

    // +Y
    { {0,1,1}, {1,1,1}, {1,1,0}, {0,1,0} },

    // -Y
    { {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1} },

    // +Z
    { {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1} },

    // -Z
    { {0,1,0}, {1,1,0}, {1,0,0}, {0,0,0} }
};

// Neighbor offsets for each face
static const glm::ivec3 NEIGHBOR_OFFSETS[6] =
{
    { 1, 0, 0 },   // +X
    {-1, 0, 0 },   // -X
    { 0, 1, 0 },   // +Y
    { 0,-1, 0 },   // -Y
    { 0, 0, 1 },   // +Z
    { 0, 0,-1 }    // -Z
};

RenderObject ChunkMesher::buildMesh(const Chunk& chunk)
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    uint32_t indexOffset = 0;

    for (int x = 0; x < Chunk::SIZE; x++)
    for (int y = 0; y < Chunk::SIZE; y++)
    for (int z = 0; z < Chunk::SIZE; z++)
    {
        const Voxel& voxel = chunk.get(x, y, z);
        if (voxel.type == 0)
            continue; // skip air

        glm::vec3 basePos = glm::vec3(x, y, z);

        // Check all 6 faces
        for (int face = 0; face < 6; face++)
        {
            glm::ivec3 n = NEIGHBOR_OFFSETS[face];
            int nx = x + n.x;
            int ny = y + n.y;
            int nz = z + n.z;

            bool neighborSolid = false;

            // Check if neighbor is inside chunk
            if (nx >= 0 && nx < Chunk::SIZE &&
                ny >= 0 && ny < Chunk::SIZE &&
                nz >= 0 && nz < Chunk::SIZE)
            {
                neighborSolid = (chunk.get(nx, ny, nz).type != 0);
            }

            if (neighborSolid)
                continue; // face hidden

            // Add the 4 vertices for this face
            for (int i = 0; i < 4; i++)
            {
                glm::vec3 v = basePos + FACE_VERTS[face][i];
                vertices.push_back(v.x);
                vertices.push_back(v.y);
                vertices.push_back(v.z);
            }

            // Add indices for two triangles
            indices.push_back(indexOffset + 0);
            indices.push_back(indexOffset + 1);
            indices.push_back(indexOffset + 2);
            indices.push_back(indexOffset + 2);
            indices.push_back(indexOffset + 3);
            indices.push_back(indexOffset + 0);

            indexOffset += 4;
        }
    }

    // Build Mesh
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" }
    };

    Mesh* mesh = new Mesh(vertices, indices, layout);

    RenderObject obj;
    obj.mesh = mesh;
    obj.color = glm::vec3(0.8f, 0.8f, 0.8f);
    obj.transform.setPosition(glm::vec3(chunk.getWorldPosition()));

    return obj;
}