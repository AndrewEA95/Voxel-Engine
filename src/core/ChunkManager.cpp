#include "ChunkManager.h"
#include "../voxel/Chunk.h"
#include "../voxel/ChunkMesher.h"
#include "../scene/Scene.h"
#include "../render/RenderObject.h"
#include "../render/Render.h"

#include "iostream"

#include <cmath>
#include <vector>

ChunkManager::ChunkManager(Scene& scene)
    : m_scene(scene)
{
    m_loadRadius = 4;
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

    for (int x = -m_loadRadius; x <= m_loadRadius; x++)
    for (int z = -m_loadRadius; z <= m_loadRadius; z++)
    {
        ChunkCoord coord = { center.x + x, 0, center.z + z };

        if (m_chunks.find(coord) == m_chunks.end())
            loadChunk(coord);
    }
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
    chunk.generateTestData();

    Render::RenderObject obj = ChunkMesher::buildMesh(chunk, *this);
    obj.isStatic = true;
    obj.entity = ECS::INVALID_ENTITY;

    std::cout << "Loading chunk at coord: "
          << coord.x << ", " << coord.y << ", " << coord.z
          << "  worldPos: "
          << chunk.getWorldPosition().x << ", "
          << chunk.getWorldPosition().y << ", "
          << chunk.getWorldPosition().z << "\n";

    std::cout << "Total chunks loaded: " << m_chunks.size() << "\n";
    

    m_scene.addObject(obj);

    m_chunks.insert({ coord, std::move(chunk) });
}

void ChunkManager::unloadChunk(const ChunkCoord& coord)
{
    m_chunks.erase(coord);
}