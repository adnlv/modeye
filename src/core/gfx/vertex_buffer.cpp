#include "../log.hpp"
#include "vertex_buffer.hpp"

modeye::gfx::vertex_buffer::vertex_buffer(size_t size, const void* data, GLenum usage)
    : m_id(0)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);

    modeye::log::trace("Generated vertex buffer (ID: {})", m_id);
}

modeye::gfx::vertex_buffer::~vertex_buffer()
{
    if (m_id != 0)
    {
        modeye::log::trace("Deleted vertex buffer (ID: {})", m_id);

        glDeleteBuffers(1, &m_id);
    }
}

modeye::gfx::vertex_buffer::vertex_buffer(vertex_buffer&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

modeye::gfx::vertex_buffer& modeye::gfx::vertex_buffer::operator=(vertex_buffer&& other) noexcept
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

void modeye::gfx::vertex_buffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void modeye::gfx::vertex_buffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

uint32_t modeye::gfx::vertex_buffer::id() const
{
    return m_id;
}
