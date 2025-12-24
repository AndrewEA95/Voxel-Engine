#define GL_SILENCE_DEPRECATION

#include "IndexBuffer.h"
#include "../../extern/glad/include/glad/glad.h"

namespace Render
{
    IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count)
        : m_Count(count)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void IndexBuffer::bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void IndexBuffer::unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

} // namespace Render