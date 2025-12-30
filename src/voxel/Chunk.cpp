#include "Chunk.h"
#include "../util/Noise.h"
#include <cmath>
#include <iostream>

Chunk::Chunk(glm::ivec3 worldPos)
    : m_WorldPos(worldPos * SIZE) // worldPos is in chunk coords, scale by SIZE
{
    std::cout << "Chunk constructor input: "
          << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << "\n";
}


void Chunk::generateTestData()
{
    glm::ivec3 base = getWorldPosition();

    int chunkX = base.x / SIZE;
    int chunkZ = base.z / SIZE;

    int height = chunkX + chunkZ; // diagonal staircase across chunks

    for (int x = 0; x < SIZE; x++)
    for (int y = 0; y < SIZE; y++)
    for (int z = 0; z < SIZE; z++)
    {
        if (y <= height)
            m_Voxels[x][y][z].type = 1; // solid
        else
            m_Voxels[x][y][z].type = 0; // air
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