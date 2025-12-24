#pragma once

#include <vector>
#include <cstdint>

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Render
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void bind() const;
        void unbind() const;

        void addVertexBuffer(const VertexBuffer& vertexBuffer);
        void setIndexBuffer(const IndexBuffer& indexBuffer);

        const IndexBuffer& getIndexBuffer() const { return *m_IndexBuffer; }

    private:
        uint32_t m_RendererID = 0;
        std::vector<VertexBuffer> m_VertexBuffers;
        const IndexBuffer* m_IndexBuffer = nullptr;
    };

} // namespace Render