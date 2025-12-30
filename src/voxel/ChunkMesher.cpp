#include "ChunkMesher.h"
#include "Chunk.h"
#include "../core/ChunkManager.h"
#include "../render/Mesh.h"
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
    int nx, int ny, int nz)
{
    const int X = Chunk::SIZE;
    const int Y = Chunk::SIZE;
    const int Z = Chunk::SIZE;

    // Determine which 2D plane we are sweeping over
    int u1, u2, v1, v2, w1, w2;

    if (nx != 0) { // X faces → sweep Y/Z
        u1 = 1; u2 = 0;  // Y axis
        v1 = 2; v2 = 0;  // Z axis
        w1 = 0; w2 = nx; // X axis normal
    }
    else if (ny != 0) { // Y faces → sweep X/Z
        u1 = 0; u2 = 1;  // X axis
        v1 = 2; v2 = 0;  // Z axis
        w1 = 1; w2 = ny; // Y axis normal
    }
    else { // Z faces → sweep X/Y
        u1 = 0; u2 = 2;  // X axis
        v1 = 1; v2 = 2;  // Y axis
        w1 = 2; w2 = nz; // Z axis normal
    }

    // Mask for merging
    struct MaskCell {
        bool visible;
        glm::vec3 color;
    };

    MaskCell mask[X][Y];

    // Sweep through the chunk
    for (int w = 0; w < Chunk::SIZE; w++)
    {
        // Build mask for this slice
        for (int u = 0; u < Chunk::SIZE; u++)
        for (int v = 0; v < Chunk::SIZE; v++)
        {
            int x = (u1 == 0 ? u : (v1 == 0 ? v : w));
            int y = (u1 == 1 ? u : (v1 == 1 ? v : w));
            int z = (u1 == 2 ? u : (v1 == 2 ? v : w));

            int nxp = x + nx;
            int nyp = y + ny;
            int nzp = z + nz;

            bool exposed = isFaceExposed(chunk, x, y, z, nxp, nyp, nzp);

            mask[u][v].visible = exposed;
            mask[u][v].color = exposed ? chunk.get(x, y, z).getColor() : glm::vec3(0);
        }

        // Greedy merge the mask
        for (int u = 0; u < Chunk::SIZE; u++)
        for (int v = 0; v < Chunk::SIZE; v++)
        {
            if (!mask[u][v].visible)
                continue;

            glm::vec3 color = mask[u][v].color;

            // Expand width
            int width = 1;
            while (u + width < Chunk::SIZE &&
                   mask[u + width][v].visible &&
                   mask[u + width][v].color == color)
            {
                width++;
            }

            // Expand height
            int height = 1;
            bool done = false;
            while (v + height < Chunk::SIZE && !done)
            {
                for (int k = 0; k < width; k++)
                {
                    if (!mask[u + k][v + height].visible ||
                        mask[u + k][v + height].color != color)
                    {
                        done = true;
                        break;
                    }
                }
                if (!done) height++;
            }

            // Mark merged area as consumed
            for (int du = 0; du < width; du++)
            for (int dv = 0; dv < height; dv++)
                mask[u + du][v + dv].visible = false;

            // Compute quad origin and axes
            glm::vec3 origin(0);
            origin[u1] = u;
            origin[v1] = v;
            origin[w1] = w + (w2 > 0 ? 1 : 0);

            glm::vec3 uVec(0), vVec(0);
            uVec[u1] = width;
            vVec[v1] = height;

            addQuad(vertices, indices, origin, uVec, vVec, color);
        }
    }
}
// ------------------------------------------------------------
// Build full greedy mesh for chunk
// ------------------------------------------------------------
Render::RenderObject ChunkMesher::buildMesh(const Chunk& chunk,
                                            const ChunkManager& manager)
{
    (void)manager;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(10000);
    indices.reserve(20000);

    greedyMeshDirection(chunk, vertices, indices, +1, 0, 0);
    greedyMeshDirection(chunk, vertices, indices, -1, 0, 0);
    greedyMeshDirection(chunk, vertices, indices,  0, +1, 0);
    greedyMeshDirection(chunk, vertices, indices,  0, -1, 0);
    greedyMeshDirection(chunk, vertices, indices,  0, 0, +1);
    greedyMeshDirection(chunk, vertices, indices,  0, 0, -1);

    // std::cout << "[buildMesh] verts=" << vertices.size()
    //           << " inds=" << indices.size() << "\n";

    Render::BufferLayout layout = {
        { Render::ShaderDataType::Float3, "a_Position" },
        { Render::ShaderDataType::Float3, "a_Color" }
    };

    Render::Mesh* mesh = new Render::Mesh(vertices, indices, layout);

    Render::RenderObject obj(mesh, glm::vec3(0.6f, 0.8f, 0.9f));
    obj.transform.setPosition(chunk.getWorldPosition()); // local mode; world matrix not used

    return obj;
}