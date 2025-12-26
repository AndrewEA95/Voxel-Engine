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
    for (int x = 0; x < SIZE; x++)
    for (int y = 0; y < SIZE; y++)
    for (int z = 0; z < SIZE; z++)
        m_Voxels[x][y][z].type = 0;

    glm::ivec3 base = getWorldPosition();

    for (int x = 0; x < SIZE; x++)
    for (int z = 0; z < SIZE; z++)
    {
        float wx = (base.x + x) * 0.05f;
        float wz = (base.z + z) * 0.05f;

        // 2-octave Perlin noise
        float h =
            perlin(wx, 0, wz) * 10.0f +
            perlin(wx * 2.0f, 0, wz * 2.0f) * 5.0f;

        int baseHeight = 8;
        int height = baseHeight + (int)h;

        for (int y = 0; y < SIZE; y++)
        {
            int wy = base.y + y;

            if (wy > height)
                continue; // leave as air (type 0)

            if (wy == height)
                m_Voxels[x][y][z].type = 1; // grass
            else if (wy >= height - 3)
                m_Voxels[x][y][z].type = 2; // dirt
            else
                m_Voxels[x][y][z].type = 3; // stone
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