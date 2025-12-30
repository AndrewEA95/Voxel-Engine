#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator()
{
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_noise.SetFrequency(0.01f); // controls hill size
}

int TerrainGenerator::getHeight(int x, int z) const
{
    float n = m_noise.GetNoise((float)x, (float)z);

    // Normalize from [-1,1] to [0,1]
    n = (n + 1.0f) * 0.5f;

    // Scale to world height
    int height = (int)(n * 40.0f + 20.0f); 
    return height;
}