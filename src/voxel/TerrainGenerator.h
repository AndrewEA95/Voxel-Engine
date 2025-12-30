#pragma once

#include "../../extern/FastNoiseLite.h"

class TerrainGenerator
{
public:
    TerrainGenerator();

    int getHeight(int x, int z) const;

private:
    FastNoiseLite m_noise;
};