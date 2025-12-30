#include "Chunk.h"
#include "TerrainGenerator.h"
#include "../voxel/Voxel.h"

// --------------------------------------
// Constructor
// --------------------------------------
Chunk::Chunk(glm::ivec3 worldPos)
    : m_position(worldPos)
{
    // Voxels are default-initialized (type = 0 = air)
}

// --------------------------------------
// Voxel access
// --------------------------------------
Voxel& Chunk::get(int x, int y, int z)
{
    return m_Voxels[x][y][z];
}

const Voxel& Chunk::get(int x, int y, int z) const
{
    return m_Voxels[x][y][z];
}

// --------------------------------------
// Terrain generation
// --------------------------------------
void Chunk::generateTerrain(const TerrainGenerator& terrain)
{
    for (int lx = 0; lx < SIZE; lx++)
    for (int lz = 0; lz < SIZE; lz++)
    {
        // Convert local chunk coords → world voxel coords
        int wx = m_position.x * SIZE + lx;
        int wz = m_position.z * SIZE + lz;

        // Heightmap lookup
        int height = terrain.getHeight(wx, wz);

        for (int ly = 0; ly < SIZE; ly++)
        {
            int wy = m_position.y * SIZE + ly;

            if (wy > height)
            {
                get(lx, ly, lz).type = 0; // air
            }
            else if (wy == height)
            {
                get(lx, ly, lz).type = 1; // grass
            }
            else if (wy > height - 4)
            {
                get(lx, ly, lz).type = 2; // dirt
            }
            else
            {
                get(lx, ly, lz).type = 3; // stone
            }
        }
    }
}