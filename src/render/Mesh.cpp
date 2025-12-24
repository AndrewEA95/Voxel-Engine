#define GL_SILENCE_DEPRECATION

#include "Render.h"
#include "Mesh.h"
#include "../../extern/glad/include/glad/glad.h"

namespace Render
{
   Mesh::Mesh(const std::vector<float>& vertices,
           const std::vector<uint32_t>& indices,
           const BufferLayout& layout)
    : m_VBO(vertices.data(), (uint32_t)(vertices.size() * sizeof(float))),
      m_IBO(indices.data(), (uint32_t)indices.size())
    {
        m_VAO.bind();          // 1. Bind VAO first
        m_VBO.bind();          // 2. Bind VBO
        m_VBO.setLayout(layout); // 3. Store layout in VBO
        m_VAO.addVertexBuffer(m_VBO); // 4. Apply layout to VAO
        m_VAO.setIndexBuffer(m_IBO);  // 5. Bind IBO to VAO
        m_VAO.unbind();
    }

    void Mesh::draw() const
    {
        m_VAO.bind();
        glDrawElements(GL_TRIANGLES, m_IBO.getCount(), GL_UNSIGNED_INT, nullptr);
    }

} // namespace Render