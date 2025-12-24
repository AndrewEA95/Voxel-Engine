#pragma once

#include <vector>

#include "../../extern/glm-1.0.2/glm/glm.hpp"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

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
        VertexArray m_VAO;
        VertexBuffer m_VBO;
        IndexBuffer m_IBO;
    };

} // namespace Render