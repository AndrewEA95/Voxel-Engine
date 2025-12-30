#pragma once

#include "Voxel.h"
#include"../../extern/glm-1.0.2/glm/glm.hpp"

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

class Chunk
{
    public:
        static constexpr int SIZE = 16;

        Chunk(glm::ivec3 worldPos);

        void generateTerrain();
        Voxel& get(int x, int y, int z);
        const Voxel& get(int x, int y, int z) const;

        glm::ivec3 m_WorldPos;
        glm::ivec3 getWorldPosition() const { return m_WorldPos; }
    
    private:
        Voxel m_Voxels[SIZE][SIZE][SIZE];
};