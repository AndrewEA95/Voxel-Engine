#pragma once

#include <vector>
#include <cstdint>
#include "BufferLayout.h"

namespace Render
{
    class Mesh
    {
    public:
        Mesh(const std::vector<float>& vertices,
             const std::vector<uint32_t>& indices,
             const BufferLayout& layout);

        void draw() const;

    private:
        uint32_t m_VAO = 0;
        uint32_t m_VBO = 0;
        uint32_t m_IBO = 0;

        uint32_t m_VertexCount = 0;
        uint32_t m_IndexCount  = 0;
    };
}