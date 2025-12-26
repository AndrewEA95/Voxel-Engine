#include "ChunkMesher.h"
#include "Chunk.h"
#include "../core/ChunkManager.h"
#include "../render/Mesh.h"
#include "../render/VertexBuffer.h"
#include "../render/Render.h"
#include <iostream>

#include <vector>

// ------------------------------------------------------------
// Helper: is voxel solid?
// ------------------------------------------------------------
static inline bool isSolid(const Chunk& chunk, int x, int y, int z)
{
    if (x < 0 || x >= Chunk::SIZE ||
        y < 0 || y >= Chunk::SIZE ||
        z < 0 || z >= Chunk::SIZE)
        return false; // out of bounds = air (for now)

    return chunk.get(x, y, z).type != 0;
}

// ------------------------------------------------------------
// Helper: is a face exposed relative to neighbor
// ------------------------------------------------------------
static inline bool isFaceExposed(const Chunk& chunk,
                                 int x, int y, int z,
                                 int nx, int ny, int nz)
{
    // Face exists only if voxel is solid AND neighbor is air
    return isSolid(chunk, x, y, z) && !isSolid(chunk, nx, ny, nz);
}

// ------------------------------------------------------------
// Add a quad (two triangles) to buffers
// ------------------------------------------------------------
static void addQuad(
    std::vector<float>& vertices,
    std::vector<uint32_t>& indices,
    const glm::vec3& origin,
    const glm::vec3& u,
    const glm::vec3& v,
    const glm::vec3& color)
{
    uint32_t startIndex = (uint32_t)(vertices.size() / 6);

    glm::vec3 p0 = origin;
    glm::vec3 p1 = origin + u;
    glm::vec3 p2 = origin + v;
    glm::vec3 p3 = origin + u + v;

    auto push = [&](const glm::vec3& p)
    {
        vertices.push_back(p.x);
        vertices.push_back(p.y);
        vertices.push_back(p.z);

        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    };
    push(p0);
    push(p1);
    push(p2);
    push(p3);

    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);

    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 3);
}

// ------------------------------------------------------------
// Greedy meshing for a single face direction (local to chunk)
// nx, ny, nz is the normal direction of the faces we emit
// ------------------------------------------------------------
static void greedyMeshDirection(
    const Chunk& chunk,
    std::vector<float>& vertices,
    std::vector<uint32_t>& indices,
    int nx, int ny, int nz) // face normal direction
{
    const int X = Chunk::SIZE;
    const int Y = Chunk::SIZE;
    const int Z = Chunk::SIZE;

    for (int x = 0; x < X; x++)
    for (int y = 0; y < Y; y++)
    for (int z = 0; z < Z; z++)
    {
        int nxp = x + nx;
        int nyp = y + ny;
        int nzp = z + nz;

        if (!isFaceExposed(chunk, x, y, z, nxp, nyp, nzp))
            continue;

        glm::vec3 origin;
        glm::vec3 uVec(0.0f);
        glm::vec3 vVec(0.0f);

        // Choose orientation and origin based on face normal
        if (nx == 1 && ny == 0 && nz == 0) // +X
        {
            origin = glm::vec3(x + 1, y, z);
            uVec   = glm::vec3(0, 1, 0); // Y
            vVec   = glm::vec3(0, 0, 1); // Z
        }
        else if (nx == -1 && ny == 0 && nz == 0) // -X
        {
            origin = glm::vec3(x, y, z);
            uVec   = glm::vec3(0, 1, 0); // Y
            vVec   = glm::vec3(0, 0, 1); // Z
        }
        else if (nx == 0 && ny == 1 && nz == 0) // +Y
        {
            origin = glm::vec3(x, y + 1, z);
            uVec   = glm::vec3(1, 0, 0); // X
            vVec   = glm::vec3(0, 0, 1); // Z
        }
        else if (nx == 0 && ny == -1 && nz == 0) // -Y
        {
            origin = glm::vec3(x, y, z);
            uVec   = glm::vec3(1, 0, 0); // X
            vVec   = glm::vec3(0, 0, 1); // Z
        }
        else if (nx == 0 && ny == 0 && nz == 1) // +Z
        {
            origin = glm::vec3(x, y, z + 1);
            uVec   = glm::vec3(1, 0, 0); // X
            vVec   = glm::vec3(0, 1, 0); // Y
        }
        else if (nx == 0 && ny == 0 && nz == -1) // -Z
        {
            origin = glm::vec3(x, y, z);
            uVec   = glm::vec3(1, 0, 0); // X
            vVec   = glm::vec3(0, 1, 0); // Y
        }
        else
        {
            continue; // should never hit
        }

        glm::vec3 color = chunk.get(x, y, z).getColor();

        addQuad(vertices, indices, origin, uVec, vVec, color);
    }
}

// ------------------------------------------------------------
// Build full greedy mesh for chunk
// ------------------------------------------------------------
Render::RenderObject ChunkMesher::buildMesh(const Chunk& chunk, const ChunkManager& manager)
{
    (void)manager; // not used yet (reserved for future neighbor-aware version)

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(10000);
    indices.reserve(20000);

    // 6 directions: ±X, ±Y, ±Z
    greedyMeshDirection(chunk, vertices, indices, +1, 0, 0); // +X
    greedyMeshDirection(chunk, vertices, indices, -1, 0, 0); // -X
    greedyMeshDirection(chunk, vertices, indices, 0, +1, 0); // +Y
    greedyMeshDirection(chunk, vertices, indices, 0, -1, 0); // -Y
    greedyMeshDirection(chunk, vertices, indices, 0, 0, +1); // +Z
    greedyMeshDirection(chunk, vertices, indices, 0, 0, -1); // -Z

    Render::BufferLayout layout = {
        { Render::ShaderDataType::Float3, "a_Position" },
        { Render::ShaderDataType::Float3, "a_Color" }
    };

    Render::Mesh* mesh = new Render::Mesh(vertices, indices, layout);

    Render::RenderObject obj(mesh, glm::vec3(0.6f, 0.8f, 0.9f));
    obj.transform.setPosition(chunk.getWorldPosition()); // local positions + chunk transform

    return obj;
}