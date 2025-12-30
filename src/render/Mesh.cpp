#define GL_SILENCE_DEPRECATION

#include "Render.h"
#include "Mesh.h"
#include "../../extern/glad/include/glad/glad.h"

namespace Render
{
   Mesh::Mesh(const std::vector<float>& vertices,
           const std::vector<uint32_t>& indices,
           const BufferLayout& layout)
    {
        m_VertexCount = (uint32_t)vertices.size();
        m_IndexCount  = (uint32_t)indices.size();

        // 🔒 If there is no geometry, make this a no-op mesh
        if (m_VertexCount == 0 || m_IndexCount == 0)
        {
            m_VAO = 0;
            m_VBO = 0;
            m_IBO = 0;
            return;
        }

        // 🔽 Everything below this is the real OpenGL setup
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER,
                    vertices.size() * sizeof(float),
                    vertices.data(),
                    GL_STATIC_DRAW);

        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    indices.size() * sizeof(uint32_t),
                    indices.data(),
                    GL_STATIC_DRAW);

        // 🔽 This is where the BufferLayout code goes
        uint32_t index = 0;
        for (const auto& element : layout.GetElements())
        {
            glEnableVertexAttribArray(index);

            uint32_t componentCount = 0;
            switch (element.Type)
            {
                case ShaderDataType::Float:  componentCount = 1; break;
                case ShaderDataType::Float2: componentCount = 2; break;
                case ShaderDataType::Float3: componentCount = 3; break;
                case ShaderDataType::Float4: componentCount = 4; break;
                default: break;
            }

            glVertexAttribPointer(
                index,
                componentCount,
                GL_FLOAT,
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)(uintptr_t)element.Offset
            );

            index++;
        }

        glBindVertexArray(0);
    }

    void Mesh::draw() const
    {
        if (m_IndexCount == 0 || m_VAO == 0)
            return; // nothing to draw, safe no-op

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

} // namespace Render