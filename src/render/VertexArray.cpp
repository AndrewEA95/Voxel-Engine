#define GL_SILENCE_DEPRECATION

#include "VertexArray.h"
#include "../../extern/glad/include/glad/glad.h"

namespace Render
{
    static GLenum shaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
                return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                return GL_INT;
            case ShaderDataType::Bool:
                return GL_BOOL;
            default:
                return GL_FLOAT;
        }
    }

    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::addVertexBuffer(const VertexBuffer& vertexBuffer)
    {
        const auto& layout = vertexBuffer.getLayout();
        if (layout.getElements().empty())
            return; // no layout, nothing to configure

        bind();
        vertexBuffer.bind();

        uint32_t index = 0;
        for (const auto& element : layout.getElements())
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index,
                element.getComponentCount(),
                shaderDataTypeToOpenGLBaseType(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                layout.getStride(),
                (const void*)(uintptr_t)element.offset
            );
            index++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::setIndexBuffer(const IndexBuffer& indexBuffer)
    {
        bind();
        indexBuffer.bind();
        m_IndexBuffer = &indexBuffer;
    }

} // namespace Render