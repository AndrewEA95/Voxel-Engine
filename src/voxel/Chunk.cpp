#include "Chunk.h"

Chunk::Chunk(glm::ivec3 worldPos) 
    : m_WorldPos(worldPos)
{

}

void Chunk::generateTestData()
{
    for(int x = 0; x < SIZE; x++)
    for(int y = 0; y < SIZE; y++)
    for(int z = 0; z < SIZE; z++)
    {
        m_Voxels[x][y][z].type = 1; //solid block
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