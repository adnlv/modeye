#include "mesh.hpp"

modeye::gfx::mesh::mesh(const std::vector<modeye::gfx::vertex>& vertices)
    : m_vertexCount(static_cast<GLsizei>(vertices.size())),
    m_vertexArray(),
    m_vertexBuffer(vertices.size() * sizeof(modeye::gfx::vertex), vertices.data(), GL_STATIC_DRAW)
{
    m_vertexArray.link_attrib(
        m_vertexBuffer,
        0,
        3,
        GL_FLOAT,
        sizeof(modeye::gfx::vertex),
        reinterpret_cast<const void*>(offsetof(modeye::gfx::vertex, position))
    );

    m_vertexArray.link_attrib(
        m_vertexBuffer,
        1,
        3,
        GL_FLOAT,
        sizeof(modeye::gfx::vertex),
        reinterpret_cast<const void*>(offsetof(modeye::gfx::vertex, normal))
    );

    m_vertexArray.link_attrib(
        m_vertexBuffer,
        2,
        2,
        GL_FLOAT,
        sizeof(modeye::gfx::vertex),
        reinterpret_cast<const void*>(offsetof(modeye::gfx::vertex, uv))
    );
}

void modeye::gfx::mesh::draw(GLenum mode) const
{
    m_vertexArray.bind();

    glDrawArrays(mode, 0, static_cast<GLsizei>(m_vertexCount));

    modeye::gfx::vertex_array::unbind();
}

const modeye::gfx::vertex_array& modeye::gfx::mesh::vertex_array() const
{
    return m_vertexArray;
}

const modeye::gfx::vertex_buffer& modeye::gfx::mesh::vertex_buffer() const
{
    return m_vertexBuffer;
}
