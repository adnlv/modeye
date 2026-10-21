#include "Mesh.hpp"

static void setupVertexAttributes(const Modeye::Gfx::VertexArray& vao, const Modeye::Gfx::VertexBuffer& vbo)
{
    vao.linkAttrib(
        vbo,
        0,
        3,
        GL_FLOAT,
        sizeof(Modeye::Gfx::Vertex),
        reinterpret_cast<const void*>(offsetof(Modeye::Gfx::Vertex, position))
    );

    vao.linkAttrib(
        vbo,
        1,
        3,
        GL_FLOAT,
        sizeof(Modeye::Gfx::Vertex),
        reinterpret_cast<const void*>(offsetof(Modeye::Gfx::Vertex, normal))
    );

    vao.linkAttrib(
        vbo,
        2,
        2,
        GL_FLOAT,
        sizeof(Modeye::Gfx::Vertex),
        reinterpret_cast<const void*>(offsetof(Modeye::Gfx::Vertex, uv))
    );
}

Modeye::Gfx::Mesh::Mesh(const std::vector<Modeye::Gfx::Vertex>& vertices)
    : m_vertexArray(),
    m_vertexBuffer(vertices.size() * sizeof(Modeye::Gfx::Vertex), vertices.data(), GL_STATIC_DRAW),
    m_indexBuffer(std::nullopt),
    m_vertexCount(static_cast<GLsizei>(vertices.size()))
{
    setupVertexAttributes(m_vertexArray, m_vertexBuffer);
}

Modeye::Gfx::Mesh::Mesh(const std::vector<Modeye::Gfx::Vertex>& vertices, const std::vector<uint32_t>& indices)
    : m_vertexArray(),
    m_vertexBuffer(vertices.size() * sizeof(Modeye::Gfx::Vertex), vertices.data(), GL_STATIC_DRAW),
    m_indexBuffer(std::in_place, indices.size(), indices.data(), GL_STATIC_DRAW),
    m_vertexCount(static_cast<GLsizei>(vertices.size()))
{
    setupVertexAttributes(m_vertexArray, m_vertexBuffer);
    m_vertexArray.bind();
    m_indexBuffer->bind();
    Modeye::Gfx::VertexArray::unbind();
}

void Modeye::Gfx::Mesh::draw(GLenum mode) const
{
    m_vertexArray.bind();

    if (m_indexBuffer.has_value()) {
        m_indexBuffer->bind();
        glDrawElements(mode, m_indexBuffer->count(), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(mode, 0, m_vertexCount);
    }

    Modeye::Gfx::VertexArray::unbind();
}

const Modeye::Gfx::VertexArray& Modeye::Gfx::Mesh::vertexArray() const
{
    return m_vertexArray;
}

const Modeye::Gfx::VertexBuffer& Modeye::Gfx::Mesh::vertexBuffer() const
{
    return m_vertexBuffer;
}

const std::optional<Modeye::Gfx::IndexBuffer>& Modeye::Gfx::Mesh::indexBuffer() const
{
    return m_indexBuffer;
}
