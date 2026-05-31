#pragma once
#include "Buffer.hpp"
#include "Vertex.hpp"
#include <memory>
#include <vector>

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices);
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    void draw(size_t vertexCount, GLenum mode) const;

    const VertexArray& vertexArray() const;
    const VertexBuffer& vertexBuffer() const;

private:
    VertexArray m_vertexArray;
    VertexBuffer m_vertexBuffer;
    GLsizei m_vertexCount;
};
