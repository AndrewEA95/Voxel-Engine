#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace Render
{
    enum class ShaderDataType
    {
        None = 0,
        Float, Float2, Float3, Float4
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
        }
        return 0;
    }

    struct BufferElement
    {
        ShaderDataType Type;
        std::string Name;
        uint32_t Size;
        uint32_t Offset;
        bool Normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {}
    };

    class BufferLayout
    {
    public:
        BufferLayout() {}

        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        inline uint32_t GetStride() const { return m_Stride; }
        inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

    private:
        void CalculateOffsetsAndStride()
        {
            uint32_t offset = 0;
            m_Stride = 0;

            for (auto& element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };
}