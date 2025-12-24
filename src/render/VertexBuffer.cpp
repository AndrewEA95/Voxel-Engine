#define GL_SILENCE_DEPRECATION

#include "VertexBuffer.h"
#include "../../extern/glad/include/glad/glad.h"

namespace Render
{
    VertexBuffer::VertexBuffer(const void* data, uint32_t size)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void VertexBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void VertexBuffer::unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

} // namespace Render