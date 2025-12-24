#pragma once

#include "Voxel.h"
#include"../../extern/glm-1.0.2/glm/glm.hpp"

class Chunk
{
    public:
        static constexpr int SIZE = 16;

        Chunk(glm::ivec3 worldPos);

        void generateTestData(); // fill with solid blocks
        Voxel& get(int x, int y, int z);
        const Voxel& get(int x, int y, int z) const;

        glm::ivec3 getWorldPosition() const { return m_WorldPos;}
    
    private:
        glm::vec3 m_WorldPos;
        Voxel m_Voxels[SIZE][SIZE][SIZE];
};