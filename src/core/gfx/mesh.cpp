#include "Mesh.hpp"

Modeye::Gfx::Mesh::Mesh(const std::vector<Modeye::Gfx::Vertex>& vertices)
    : m_vertexCount(static_cast<GLsizei>(vertices.size())),
    m_vertexArray(),
    m_vertexBuffer(vertices.size() * sizeof(Modeye::Gfx::Vertex), vertices.data(), GL_STATIC_DRAW)
{
    m_vertexArray.linkAttrib(
        m_vertexBuffer,
        0,
        3,
        GL_FLOAT,
        sizeof(Modeye::Gfx::Vertex),
        reinterpret_cast<const void*>(offsetof(Modeye::Gfx::Vertex, position))
    );

    m_vertexArray.linkAttrib(
        m_vertexBuffer,
        1,
        3,
        GL_FLOAT,
        sizeof(Modeye::Gfx::Vertex),
        reinterpret_cast<const void*>(offsetof(Modeye::Gfx::Vertex, normal))
    );

    m_vertexArray.linkAttrib(
        m_vertexBuffer,
        2,
        2,
        GL_FLOAT,
        sizeof(Modeye::Gfx::Vertex),
        reinterpret_cast<const void*>(offsetof(Modeye::Gfx::Vertex, uv))
    );
}

void Modeye::Gfx::Mesh::draw(GLenum mode) const
{
    m_vertexArray.bind();

    glDrawArrays(mode, 0, static_cast<GLsizei>(m_vertexCount));

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
