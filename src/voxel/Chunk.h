#pragma once

#include "TerrainGenerator.h"
#include "Voxel.h"
#include "../../extern/glm-1.0.2/glm/glm.hpp"

// -----------------------------
// ChunkCoord + hashing
// -----------------------------
struct ChunkCoord
{
    int x, y, z;

    bool operator==(const ChunkCoord& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const ChunkCoord& other) const
    {
        return !(*this == other);
    }
};

namespace std
{
    template<>
    struct hash<ChunkCoord>
    {
        size_t operator()(const ChunkCoord& c) const noexcept
        {
            size_t h1 = std::hash<int>()(c.x);
            size_t h2 = std::hash<int>()(c.y);
            size_t h3 = std::hash<int>()(c.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

// -----------------------------
// Chunk class
// -----------------------------
class Chunk
{
public:
    static constexpr int SIZE = 16;

    // worldPos is in CHUNK COORDINATES (not voxel coordinates)
    Chunk(glm::ivec3 worldPos);

    // Chunk coordinate in the world (chunk-space)
    glm::ivec3 m_position;

    // Terrain generation
    void generateTerrain(const TerrainGenerator& terrain);

    // Voxel access
    Voxel& get(int x, int y, int z);
    const Voxel& get(int x, int y, int z) const;

    // World voxel position (top-left corner of chunk)
    glm::ivec3 getWorldPosition() const
    {
        return glm::ivec3(
            m_position.x * SIZE,
            m_position.y * SIZE,
            m_position.z * SIZE
        );
    }

private:
    // 3D voxel storage
    Voxel m_Voxels[SIZE][SIZE][SIZE];
};