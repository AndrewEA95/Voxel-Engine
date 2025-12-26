#pragma once
#include <cstdint>
#include "../../extern/glm-1.0.2/glm/glm.hpp"

struct Voxel
{
    uint8_t type = 0; //0 = air, 1 = solid block (for now)

    glm::vec3 getColor() const
    {
        switch (type)
        {
            case 1: return glm::vec3(0.2f, 0.8f, 0.3f); // grass
            case 2: return glm::vec3(0.55f, 0.27f, 0.07f); // dirt
            case 3: return glm::vec3(0.5f); // stone
            default: return glm::vec3(0.0f); // air or unknown
        }
    }
};