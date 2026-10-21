#pragma once
#include "IndexBuffer.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include <optional>
#include <vector>

namespace Modeye::Gfx
{

class Mesh
{
public:
    Mesh(const std::vector<Modeye::Gfx::Vertex>& vertices);
    Mesh(const std::vector<Modeye::Gfx::Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    void draw(GLenum mode) const;

    const Modeye::Gfx::VertexArray& vertexArray() const;
    const Modeye::Gfx::VertexBuffer& vertexBuffer() const;
    const std::optional<Modeye::Gfx::IndexBuffer>& indexBuffer() const;

private:
    VertexArray m_vertexArray;
    VertexBuffer m_vertexBuffer;
    std::optional<IndexBuffer> m_indexBuffer;
    GLsizei m_vertexCount;
};

}
