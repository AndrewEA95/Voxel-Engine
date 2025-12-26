#include "Chunk.h"
#include <cmath>

Chunk::Chunk(glm::ivec3 worldPos)
    : m_WorldPos(worldPos * SIZE) // worldPos is in chunk coords, scale by SIZE
{
}

void Chunk::generateTestData()
{
    for (int x = 0; x < SIZE; x++)
    for (int y = 0; y < SIZE; y++)
    for (int z = 0; z < SIZE; z++)
        m_Voxels[x][y][z].type = 0;

    // Simple hill: height increases with world X and Z
    glm::ivec3 base = getWorldPosition();

    for (int x = 0; x < SIZE; x++)
    for (int z = 0; z < SIZE; z++)
    {
        int wx = base.x + x;
        int wz = base.z + z;

        int height = (wx + wz) % 20; // simple slope

        for (int y = 0; y < SIZE; y++)
        {
            int wy = base.y + y;
            if (wy <= height)
                m_Voxels[x][y][z].type = 1;
        }
    }
}

Voxel& Chunk::get(int x, int y, int z)
{
    return m_Voxels[x][y][z];
}

const Voxel& Chunk::get(int x, int y, int z) const
{
    return m_Voxels[x][y][z];
}