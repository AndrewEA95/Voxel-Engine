#include "Chunk.h"
#include "../util/Noise.h"
#include <iostream>

Chunk::Chunk(glm::ivec3 worldPos)
    : m_WorldPos(worldPos * SIZE) // worldPos is in chunk coords, scale by SIZE
{
    
}


void Chunk::generateTerrain()
{
    glm::ivec3 base = getWorldPosition();

    const float frequency = 0.02f;
    const float amplitude = 20.0f;
    const float baseHeight = 30.0f;

    for (int x = 0; x < SIZE; x++)
    for (int z = 0; z < SIZE; z++)
    {
        float wx = float(base.x + x);
        float wz = float(base.z + z);

        float n = perlin(wx * frequency, 0.0f, wz * frequency);
        n = (n + 1.0f) * 0.5f;

        int height = int(baseHeight + n * amplitude);
        if (base.x == 0 && base.z == 0 && x == 0 && z == 0)
        {
            std::cout << "[terrain] base=(" << base.x << "," << base.y << "," << base.z
                    << ") height=" << height << "\n";
        }

        for (int y = 0; y < SIZE; y++)
        {
            int wy = base.y + y;

            m_Voxels[x][y][z].type = (wy <= height) ? 1 : 0;
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