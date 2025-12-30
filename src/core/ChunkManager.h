#pragma once
#include <unordered_map>
#include "../voxel/Chunk.h"
#include "../scene/Scene.h"

struct ChunkCoord
{
    int x, y, z;

    bool operator==(const ChunkCoord& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct ChunkCoordHash
{
    size_t operator()(const ChunkCoord& c) const
    {
        return (std::hash<int>()(c.x) * 73856093) ^
               (std::hash<int>()(c.y) * 19349663) ^
               (std::hash<int>()(c.z) * 83492791);
    }
};

class ChunkManager
{
public:
    ChunkManager(Scene& scene);

    void update(const glm::vec3& cameraPos);

    Voxel getVoxelAt(int wx, int wy, int wz) const;
    const Chunk* getChunk(const ChunkCoord& coord) const;

private:
    Scene& m_scene;
    ChunkCoord m_lastCenter = { 999999, 999999, 999999 };

    std::unordered_map<ChunkCoord, Chunk, ChunkCoordHash> m_chunks;

    int m_loadRadius; // chunks in each direction
    int m_verticalRadius;

    void loadChunk(const ChunkCoord& coord);
    void unloadFarChunks(const ChunkCoord& center);
    void unloadChunk(const ChunkCoord& coord);

    ChunkCoord worldToChunk(const glm::vec3& pos);
};