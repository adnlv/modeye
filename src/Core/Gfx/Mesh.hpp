#pragma once
#include "Vertex.hpp"
#include "VertexArray.hpp"

namespace Modeye::Gfx
{

class Mesh
{
public:
    Mesh(const std::vector<Modeye::Gfx::Vertex>& vertices);
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    void draw(GLenum mode) const;

    const Modeye::Gfx::VertexArray& vertexArray() const;
    const Modeye::Gfx::VertexBuffer& vertexBuffer() const;

private:
    VertexArray m_vertexArray;
    VertexBuffer m_vertexBuffer;
    GLsizei m_vertexCount;
};

}
