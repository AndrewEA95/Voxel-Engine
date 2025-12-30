#include "ChunkManager.h"
#include "../voxel/Chunk.h"
#include "../voxel/ChunkMesher.h"
#include "../scene/Scene.h"
#include "../render/RenderObject.h"

#include "iostream"

#include <cmath>
#include <vector>

ChunkManager::ChunkManager(Scene& scene)
    : m_scene(scene)
{
    m_loadRadius = 2;
    m_verticalRadius = 4;
}

ChunkCoord ChunkManager::worldToChunk(const glm::vec3& pos)
{
        return {
        (int)std::floor(pos.x / Chunk::SIZE),
        (int)std::floor(pos.y / Chunk::SIZE),
        (int)std::floor(pos.z / Chunk::SIZE)
    };
}

void ChunkManager::update(const glm::vec3& cameraPos)
{
    ChunkCoord center = worldToChunk(cameraPos);

     //std::cout << "[update] center = " << center.x << ", " << center.y << ", " << center.z << "\n";

    // Only update if the camera entered a new *horizontal* chunk
    if (center.x == m_lastCenter.x &&
        center.y == m_lastCenter.y &&
        center.z == m_lastCenter.z)
        return;

    std::cout << "Camera center chunk: " << center.x << ", " << center.z << "\n";

    m_lastCenter = center;

    for (int x = -m_loadRadius; x <= m_loadRadius; x++)
    for (int y = -m_verticalRadius; y <= m_verticalRadius; y++)
    for (int z = -m_loadRadius; z <= m_loadRadius; z++)
    {
        ChunkCoord coord = { center.x + x, center.y + y, center.z + z };

        if (m_chunks.find(coord) == m_chunks.end())
            loadChunk(coord);
    }

    // Unload chunks that are too far away
    unloadFarChunks(center);
}

const Chunk* ChunkManager::getChunk(const ChunkCoord& coord) const
{
    auto it = m_chunks.find(coord);
    if (it == m_chunks.end())
        return nullptr;
    return &it->second;
}

Voxel ChunkManager::getVoxelAt(int wx, int wy, int wz) const
{
    int cx = (int)std::floor((float)wx / Chunk::SIZE);
    int cy = (int)std::floor((float)wy / Chunk::SIZE);
    int cz = (int)std::floor((float)wz / Chunk::SIZE);

    ChunkCoord coord{ cx, cy, cz };

    auto it = m_chunks.find(coord);
    if (it == m_chunks.end())
        return Voxel{ 0 };

    const Chunk& chunk = it->second;
    glm::ivec3 base = chunk.getWorldPosition();

    int lx = wx - base.x;
    int ly = wy - base.y;
    int lz = wz - base.z;

    if (lx < 0 || lx >= Chunk::SIZE ||
        ly < 0 || ly >= Chunk::SIZE ||
        lz < 0 || lz >= Chunk::SIZE)
        return Voxel{ 0 };

    return chunk.get(lx, ly, lz);
}

void ChunkManager::loadChunk(const ChunkCoord& coord)
{
    Chunk chunk(glm::ivec3(coord.x, coord.y, coord.z));
    chunk.generateTerrain();

    Render::RenderObject obj = ChunkMesher::buildMesh(chunk, *this);
    obj.isStatic = true;
    obj.entity = ECS::INVALID_ENTITY;

    // Restore debug chunk coloring
    obj.color = glm::vec3(
        (coord.x & 1) ? 1.0f : 0.2f,
        (coord.z & 1) ? 1.0f : 0.2f,
        (coord.y & 1) ? 0.8f : 0.3f
    );

    m_scene.addObject(obj);
    m_chunks.insert({ coord, std::move(chunk) });
}

void ChunkManager::unloadFarChunks(const ChunkCoord& center)
{
    std::vector<ChunkCoord> toRemove;

    for (auto& [coord, chunk] : m_chunks)
    {
        int dx = coord.x - center.x;
        int dy = coord.y - center.y;
        int dz = coord.z - center.z;

        if (std::abs(dx) > m_loadRadius ||
            std::abs(dy) > m_verticalRadius ||
            std::abs(dz) > m_loadRadius)
        {
            toRemove.push_back(coord);
        }
    }

    for (auto& coord : toRemove)
        unloadChunk(coord);
}

void ChunkManager::unloadChunk(const ChunkCoord& coord)
{
    m_chunks.erase(coord);
}