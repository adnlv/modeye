#include "../Log.hpp"
#include "VertexBuffer.hpp"

Modeye::Gfx::VertexBuffer::VertexBuffer(size_t size, const void* data, GLenum usage)
    : m_id(0)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);

    Modeye::Log::trace("Generated vertex buffer (ID: {})", m_id);
}

Modeye::Gfx::VertexBuffer::~VertexBuffer()
{
    if (m_id != 0)
    {
        Modeye::Log::trace("Deleted vertex buffer (ID: {})", m_id);

        glDeleteBuffers(1, &m_id);
    }
}

Modeye::Gfx::VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

Modeye::Gfx::VertexBuffer& Modeye::Gfx::VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
    if (this != &other)
    {
        if (m_id != 0)
        {
            glDeleteBuffers(1, &m_id);
        }

        m_id = other.m_id;
        other.m_id = 0;
    }

    return *this;
}

void Modeye::Gfx::VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Modeye::Gfx::VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

uint32_t Modeye::Gfx::VertexBuffer::id() const
{
    return m_id;
}
