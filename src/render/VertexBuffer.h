#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "../../extern/glm-1.0.2/glm/glm.hpp"

namespace Render
{
    enum class ShaderDataType
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    static uint32_t shaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::Mat3:   return 4 * 3 * 3;
            case ShaderDataType::Mat4:   return 4 * 4 * 4;
            case ShaderDataType::Int:    return 4;
            case ShaderDataType::Int2:   return 4 * 2;
            case ShaderDataType::Int3:   return 4 * 3;
            case ShaderDataType::Int4:   return 4 * 4;
            case ShaderDataType::Bool:   return 1;
            default:                     return 0;
        }
    }

    struct BufferElement
    {
        std::string name;
        ShaderDataType type;
        uint32_t size;
        uint32_t offset;
        bool normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : name(name),
              type(type),
              size(shaderDataTypeSize(type)),
              offset(0),
              normalized(normalized)
        {
        }

        uint32_t getComponentCount() const
        {
            switch (type)
            {
                case ShaderDataType::Float:  return 1;
                case ShaderDataType::Float2: return 2;
                case ShaderDataType::Float3: return 3;
                case ShaderDataType::Float4: return 4;
                case ShaderDataType::Mat3:   return 3 * 3;
                case ShaderDataType::Mat4:   return 4 * 4;
                case ShaderDataType::Int:    return 1;
                case ShaderDataType::Int2:   return 2;
                case ShaderDataType::Int3:   return 3;
                case ShaderDataType::Int4:   return 4;
                case ShaderDataType::Bool:   return 1;
                default:                     return 0;
            }
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() = default;

        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_Elements(elements)
        {
            calculateOffsetsAndStride();
        }

        const std::vector<BufferElement>& getElements() const { return m_Elements; }
        uint32_t getStride() const { return m_Stride; }

    private:
        void calculateOffsetsAndStride()
        {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements)
            {
                element.offset = offset;
                offset += element.size;
                m_Stride += element.size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(const void* data, uint32_t size);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;

        void setLayout(const BufferLayout& layout) { m_Layout = layout; }
        const BufferLayout& getLayout() const { return m_Layout; }

    private:
        uint32_t m_RendererID = 0;
        BufferLayout m_Layout;
    };

} // namespace Render