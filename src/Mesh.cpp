#include "Mesh.hpp"
#include "Vertex.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices)
    : m_vertexCount(static_cast<GLsizei>(vertices.size())),
    m_vertexArray(),
    m_vertexBuffer(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW)
{
    m_vertexArray.linkAttrib(
        m_vertexBuffer,
        0,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, position))
    );
    
    m_vertexArray.linkAttrib(
        m_vertexBuffer,
        1,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, normal))
    );
    
    m_vertexArray.linkAttrib(
        m_vertexBuffer,
        2, 
        2, 
        GL_FLOAT, 
        sizeof(Vertex), 
        reinterpret_cast<const void*>(offsetof(Vertex, uv))
    );
}

void Mesh::draw(GLenum mode) const
{
    m_vertexArray.bind();

    glDrawArrays(mode, 0, static_cast<GLsizei>(m_vertexCount));

    VertexArray::unbind();
}

const VertexArray& Mesh::vertexArray() const
{
    return m_vertexArray;
}

const VertexBuffer& Mesh::vertexBuffer() const
{
    return m_vertexBuffer;
}
